#ifndef _TEST_TESTSQLITE_2020_03_19_14_48_30_H
#define _TEST_TESTSQLITE_2020_03_19_14_48_30_H

namespace TestSqlite {

bool doTrans(ws::sqlite::IDB* db) {
	auto v = db->query("select * from goods limit 0,6");
	v = db->query("select * from goods limit 0,100");

	ws::FastJson json;
	auto i = 0;
	while(v->next()) {
		json[i]["id"] = v->asI64("_id");
		json[i]["name"] = v->asUTF8("_name");
		json[i]["price"] = v->asI64("_price");
		++i;
	}
	
	ws::print(json.toPrettyString());
	return true;
}

bool testExec() {
	auto db = ws::sqlite::createDB();

	auto dbpath = ws::System::getAppDirectory().append("maomao.db");
	if(!db->open(dbpath, "123456")) {
		return false;
	}

	ws::ustring sql = "create table if not exists goods(\n"
		"_id int primary key not null,\n"
		"_name blob not null,\n"
		"_price int not null)";
	if(!db->exec(sql)) {
		return false;
	}

	ws::ustring gname("adbABC");
	ws::i64 va = 0;
	sql = fmt::format(
		"insert into goods(_id, _name, _price) \n"
		"values ({}, '{}', {}), \n"
		"({}, '{}', {})", 45, gname, 100,
		34, ws::sqlite::quote("云烟'[]%()_"), 200);
	if(!db->exec(sql, &va)) {
		return false;
	}

	db->transaction(std::bind(doTrans, db));
	return true;
}

}

TEST_CASE("sqlite") {
	CHECK(TestSqlite::testExec());
	_getch();
}

#endif //_TEST_TESTSQLITE_2020_03_19_14_48_30_H