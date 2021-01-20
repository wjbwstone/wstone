#ifndef _TEST_TESTJSON_2020_02_27_13_45_34_H
#define _TEST_TESTJSON_2020_02_27_13_45_34_H

//#include <boost/any.hpp>

namespace TestJson {

bool testJsonHelper() {
	FastJson root;
	ws::JsonHelper::parseFile("F:\\lizhi\\2.json", root);
	auto bs = root.toShortString().size();
	auto sizes = root.arraySize();

	ws::JsonHelper::writeToConsole(root);
	ws::JsonHelper::writeToFile("F:\\lizhi\\maomao.json", root);

	FastJson sroot;
	ws::JsonHelper::parseFile("F:\\lizhi\\maomao.json", sroot);
	sroot["dong"] = "万受得了肺结核阿拉山口的计费";

	FastJson eroot;
	ws::JsonHelper::writeToEncryptFile("F:\\lizhi\\l.json", root);
	ws::JsonHelper::writeToConsole(eroot);

	return true;
}

void printValue(const rapidjson::Value& v) {
	rapidjson::StringBuffer buffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
	v.Accept(writer);
	if(writer.IsComplete()) {
		ws::nprint(buffer.GetString(), buffer.GetSize());
	}
}

bool tfastJson() {
	FastJson jconst;
	const auto& vc = jconst[1];
	ws::print(vc.toPrettyString());
	auto vb = vc[0];

	auto fpath = ws::System::getAppDirectory() + "language.bin";
	ws::Language lan;
	lan.loadFile(fpath);
	ws::print(lan.text("通用", "buttonPopComfirm"));

	ws::ustring stringValue = ("klwjflkje");

	JsonWriter jwr;
	jwr.startArray();
	jwr.append("skflelkjalksdjfe");
	jwr.append(24234234);
	jwr.endArray();
	ws::print(jwr.toString());

	FastJson jparse;
	{
		FastJson jo;
		jo = 1242345;
		jparse = std::move(jo);
	}
	jparse.parse(jwr.toString());
	jparse[1] = 234234234;
	auto jv = jparse[5].asString();
	ws::print(jparse.toPrettyString());

	FastJson jobject;
	jobject["mao"] = 23444;
	jobject["mao2"] = "slfje";
	jobject["mao1"] = stringValue;
	jobject["mao5"]["mao03"] = 2453245;
	jobject["mao5"]["mao01"] = 2453245;
	ws::print(jobject["mao"].toPrettyString());

	FastJson jarray;
	jarray.append(23534);
	jarray.append("23534").append(3645667.2345);
	jarray.append(true);
	jarray.append(jobject);
	ws::print(jarray.toPrettyString());

	jarray.empty();

	ws::vprint("{} ", jarray.arraySize());

	jobject = std::move(jarray);
	ws::print(jobject.toPrettyString());
	jarray.append(23453456);
	for(ws::u32 i = 0; i < jarray.arraySize(); ++i) {
		const auto& v = jarray[i];
		ws::vprint("{}", v.toPrettyString());
	}

	jobject.clear();
	ws::print(jobject.toPrettyString());

	auto vv = jobject[nullptr];
	vv = "slkfjelkj";
	ws::print(vv.toPrettyString());

	return true;
	getchar();
}

}

struct MyStruct {
	void operator()() {
		ws::print("functor");
	}
};


TEST_CASE("FastJson") {
	CHECK(TestJson::tfastJson());
	getchar();
}


#endif //_TEST_TESTJSON_2020_02_27_13_45_34_H