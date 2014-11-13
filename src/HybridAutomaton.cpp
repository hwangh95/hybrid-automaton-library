#include "hybrid_automaton/HybridAutomaton.h"

#include "hybrid_automaton/DescriptionTreeNode.h"

#include <sstream>

namespace ha {

void HybridAutomaton::registerController(const std::string& crtl_name, ControllerCreator cc) {
  std::map<std::string, HybridAutomaton::ControllerCreator>& controller_type_map = getControllerTypeMap();
  assert ( controller_type_map.find(crtl_name) == controller_type_map.end() );
  controller_type_map[crtl_name] = cc;
}

void HybridAutomaton::registerControlSet(const std::string& crtl_name, ControlSetCreator cc) {
  std::map<std::string, HybridAutomaton::ControlSetCreator>& controlset_type_map = getControlSetTypeMap();
  assert ( controlset_type_map.find(crtl_name) == controlset_type_map.end() );
  controlset_type_map[crtl_name] = cc;
}

Controller::Ptr HybridAutomaton::createController(const std::string& crtl_name) {
  std::map<std::string, HybridAutomaton::ControllerCreator>& controller_type_map = getControllerTypeMap();
  std::map<std::string, HybridAutomaton::ControllerCreator>::iterator it = controller_type_map.find(crtl_name);
  if ( it == controller_type_map.end() ) {
	  std::stringstream ss;
	  ss << "[HybridAutomaton::createController] Controller not registered: " << crtl_name;
	  throw ss.str();
  }
  return (*(it->second))();
}

void HybridAutomaton::addControlMode(const ControlMode::Ptr& control_mode) {
	boost::add_vertex(control_mode->getName(), control_mode, _graph);
}

void HybridAutomaton::addControlSwitch(const std::string& source_mode, const ControlSwitch::Ptr& control_switch, const std::string& target_mode) {
	boost::add_edge_by_label(source_mode, target_mode, control_switch, _graph);
}

void HybridAutomaton::addControlSwitchAndMode(const std::string& source_mode, const ControlSwitch::Ptr& control_switch, const ControlMode::Ptr& target_mode) {
	boost::add_vertex(target_mode->getName(), target_mode, _graph);
	boost::add_edge_by_label(source_mode, target_mode->getName(), control_switch, _graph);
}

::Eigen::VectorXd HybridAutomaton::step(const double& t) {
	// check if any out-going jump condition is true
	::std::pair<OutEdgeIterator, OutEdgeIterator> out_edges = ::boost::out_edges(_graph.vertex(_current_control_mode->getName()), _graph);
	for(; out_edges.first != out_edges.second; ++out_edges.first) {
		SwitchHandle switch_handle = *out_edges.first;
		ControlSwitch::Ptr control_switch = _graph[switch_handle];

		if (control_switch->isActive())
		{
			// switch to the next control mode
			ModeHandle mode_handle = boost::target(switch_handle, _graph);

			_current_control_mode->deactivate();
			_current_control_mode = _graph.graph()[mode_handle];
			_current_control_mode->activate();

			// CE: We still need to encapsulate this functionality:
			//		if (!_activeMotionBehaviour->replaceControllers((MotionBehaviour*) edges[0]))
			//		{
			//			_activeMotionBehaviour = (MotionBehaviour*) edges[0];
			//		}
			break;
		}
	}

	return _current_control_mode->step(t); 
}

void HybridAutomaton::serialize(DescriptionTreeNode::Ptr& tree) const {
	throw "not implemented";
}

void HybridAutomaton::deserialize(const DescriptionTreeNode::Ptr tree){
	//tree.getAttribute(std::string("name"));
	//name = tree.getAttribute("name");
}

void HybridAutomaton::setName(const std::string& name) {
			_name = name;
		}

const std::string& HybridAutomaton::getName() const {
	return _name;
}

}
