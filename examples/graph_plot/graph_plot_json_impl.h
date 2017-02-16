/*
 * This file is auto-generated using json-maker
 */

#include "graph_plot.h"
#include <json>

namespace AutoJson {
template<>
AutoJson::Json::Json(const ::GraphPlot::Edge& rhs) : type(JsonType::OBJECT), content(new std::map<std::string, Json>()) {
	(*this)["source"] = rhs.source;
	(*this)["target"] = rhs.target;
	(*this)["isDirected"] = rhs.is_directed;
}

template<>
AutoJson::Json::operator ::GraphPlot::Edge() {
	::GraphPlot::Edge obj;
	obj.source = this->operator[]("source").Get<string>();
	obj.target = this->operator[]("target").Get<string>();
	obj.is_directed = this->operator[]("isDirected").Get<int>();
	return obj;
}
}  //namespace AutoJson

namespace AutoJson {
template<>
AutoJson::Json::Json(const ::GraphPlot::Node& rhs) : type(JsonType::OBJECT), content(new std::map<std::string, Json>()) {
	(*this)["xPos"] = rhs.x_pos;
	(*this)["yPos"] = rhs.y_pos;
}

template<>
AutoJson::Json::operator ::GraphPlot::Node() {
	::GraphPlot::Node obj;
	obj.x_pos = this->operator[]("xPos").Get<int>();
	obj.y_pos = this->operator[]("yPos").Get<int>();
	return obj;
}
}  //namespace AutoJson

namespace AutoJson {
template<>
AutoJson::Json::Json(const ::GraphPlot& rhs) : type(JsonType::OBJECT), content(new std::map<std::string, Json>()) {
	(*this)["xSize"] = rhs.x_size;
	(*this)["ySize"] = rhs.y_size;
	(*this)["allEdges"] = rhs.all_edges;
	(*this)["nodeInfo"] = rhs.node_info;
}

template<>
AutoJson::Json::operator ::GraphPlot() {
	::GraphPlot obj;
	obj.x_size = this->operator[]("xSize").Get<int>();
	obj.y_size = this->operator[]("ySize").Get<int>();
	obj.all_edges = this->operator[]("allEdges").Get<vector<::GraphPlot::Edge>>();
	obj.node_info = this->operator[]("nodeInfo").Get<map<string, ::GraphPlot::Node>>();
	return obj;
}
}  //namespace AutoJson


