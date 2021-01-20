#ifndef _TEST_TESTMYSQL_2020_03_19_14_47_14_H
#define _TEST_TESTMYSQL_2020_03_19_14_47_14_H

using namespace ws::mysql;

bool doTrans(IDBManager* dbmanager) {
	return true;
}

void dbLog(ws::SimpleLogSink::Level, const i8* data, u32 bytes) {
	ws::nprint(data, bytes);
}

void doPrintMarket(IDBManager* db) {
	ws::i64 total = 0;
	ws::mysql::tableMetaData tTables;

	db->exec("repair table proc", &total);
	db->exec("set global max_allowed_packet = 10*1024*1024", &total);
	auto table = db->query("select SQL_CALC_FOUND_ROWS * from market limit 0, 2", &total);
	if(!db->getTableMetaData(tTables, "market")) {
		return;
	}

	const auto& itT = tTables.begin()->second;
	while(table->next()) {
		ws::FastJson jfrow;
		for(ws::u32 i = 0; i < itT.size(); ++i) {
			const auto& n = itT[i]._name.c_str();
			switch(itT[i]._type) {
			case ws::mysql::Field::kTypeBool:
				jfrow[n] = table->asBool(n);
				break;
			case ws::mysql::Field::kTypeI32:
				jfrow[n] = table->asI32(n);
				break;
			case ws::mysql::Field::kTypeU32:
				jfrow[n] = table->asU32(n);
				break;
			case ws::mysql::Field::kTypeI64:
				jfrow[n] = table->asI64(n);
				break;
			case ws::mysql::Field::kTypeU64:
				jfrow[n] = table->asU64(n);
				break;
			case ws::mysql::Field::kTypeUTF8:
				jfrow[n] = table->asCUTF8(n);
				break;
			default: break;
			}
		}

		ws::print(jfrow.toPrettyString());
	}
}

bool buildPUBGBillBoard(IDBManager* db) {
	ws::print(db->escape(ustring("1%@#~\/", 7)));
	ws::i64 toa;
	auto table = db->query("select *  from feecharge", &toa);
	while(table->next()) {
		auto rank = table->asU32("_rank");
		if(rank <= 10) {
			rank = 1;
		} else {
			rank = 0;
		}

		auto gid = table->asU64("_gameplayerid");
		auto kill = table->asU32("_kill");

		std::string sql;
		auto stable = db->query(fmt::format("select * from billboardpubgrecord "
			"where _gameplayerid = {}", gid));
		if(stable->empty()) {
			sql = fmt::format("insert into billboardpubgrecord(_id, _gameplayerid, "
				"_kill, _rank, _createtime, _version) "
				"values({}, {}, {}, {}, '{}', {})",
				ws::System::getUniqueID(), gid, kill,
				rank, ws::Time::toDatetimeString(),
				-1);

		} else {
			sql = fmt::format("update billboardpubgrecord set _kill = _kill + {}, "
				"_rank = _rank + {}, _version = {} where _gameplayerid = {}",
				kill, rank, -1, gid);
		}
		db->exec(sql);
	}

	return true;
}

struct Items {
	ws::ustring op;
	bool valid;
	ws::ustring name;
};

void doB(IDBManager* db) {

	std::map<ws::ustring, Items> vs;
	auto table = db->query("select * from yx_bar_app_op_log "
		"where operator != 'gouweihua' order by create_time desc");

	while(table->next()) {
		auto barid = table->asUTF8("bar_id");
		auto status = table->asUTF8("app_op_way");
		auto name = table->asUTF8("bar_name");
		auto found =  vs.find(barid);

		if(vs.end() == found) {
			Items v;
			v.op = status;
			v.valid = false;
			vs.emplace(barid, v);

		} else {
			if("open" == found->second.op) {
				if("close" == status) {
					found->second.valid = true;
					found->second.name = name;
				}
			}
		}
	}

	ws::FastJson jroot;
	for(auto it : vs) {
		if(it.second.valid) {
			auto v = jroot[it.first.c_str()];
			v["status"] = it.second.op;
			v["name"] = it.second.name;
		}
	}

	auto s = jroot.memberSize();
	//ws::JsonHelper::writeToFile("F:\\mao.json", jroot);
	db->useDataBase("yx_newboss");

	std::map<ws::ustring, bool> vs2;
	table = db->query("select * from yx_desktop_app where "
		"enabled = 0 and app_type = 100");
	while(table->next()) {
		vs2.emplace(table->asUTF8("bar_id"), table->asBool("enabled"));
	}

	for(const auto it : vs) {
		if(vs2.end() != vs2.find(it.first) && jroot.hasMember(it.first)) {
			MLOGE("错误数据，需要认证 : {}", 
				jroot[it.first.c_str()]["name"].asString());
		}
	}

	getchar();
}

#include <foundation/src/mysql/MySQLCell.h>

TEST_CASE("mysql") {
	auto db = createDBManager();
	db->setLogPrinter(dbLog);
	db->conectDB("127.0.0.1", "root", 
		"MDM0MzA3MzI1167r9X4gaCBaHxRw+uWJgw==", 4567);
	db->useDataBase("mmaster");

	/*db->conectDB("rm-bp1vu65w3gxw1jp7m9o.mysql.rds.aliyuncs.com", "dbwd",
		"EO8vBVLPwA2Xclh5tSZM", 3306);
	db->useDataBase("yx_system_log");
	doB(db);*/

	doPrintMarket(db);
	db->transaction(std::bind(&buildPUBGBillBoard, db));
	//buildPUBGBillBoard(db);

	getchar();
	destroyDBManager(db);
}

#endif //_TEST_TESTMYSQL_2020_03_19_14_47_14_H