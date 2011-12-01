#include "OpSpaceMilestone.h"

#include <iostream>
#include <string>

#include "tinyxml.h"
#include "XMLDeserializer.h"

using namespace std;

#define RADIUS 0.4

OpSpaceMilestone::OpSpaceMilestone() :
Milestone(),
motion_behaviour_(NULL),
//object_id_(-1),
posi_ori_selection_(POSITION_SELECTION)	// Default value in the default constructor
{
}

OpSpaceMilestone::OpSpaceMilestone(std::vector<double>& posi_ori_value, PosiOriSelector posi_ori_selection, MotionBehaviour * motion_behaviour, std::vector<double>& region_convergence_radius) :
Milestone()
//object_id_(object_id)
{
	posi_ori_selection_ = posi_ori_selection;
	configuration_.resize(6, -1.0);
	region_convergence_radius_.resize(6, -1.0);
	int counter_offset = -1;
	int max_count = -1;
	switch(posi_ori_selection_){
		case POSITION_SELECTION:
			counter_offset = 0;
			max_count = 3;
			break;
		case ORIENTATION_SELECTION:
			counter_offset = 3;
			max_count = 6;
			break;
		case POS_AND_ORI_SELECTION:
			counter_offset = 0;
			max_count = 6;
			break;
		default:
			throw std::string("ERROR [OpSpaceMilestone::OpSpaceMilestone(std::vector<double>& posi_ori_value, PosiOriSelection posi_ori_selection, MotionBehaviour * motion_behaviour, std::vector<double>& region_convergence_radius,int object_id)]: Wrong posi_ori_selection value.");
			break;
	}
	int i = counter_offset;
	int n = 0;
	for(; i<max_count; i++, n++){
		configuration_[i] = posi_ori_value[n];
		region_convergence_radius_[i] = region_convergence_radius[n];
	}
	if(motion_behaviour)
	{
		this->motion_behaviour_ = motion_behaviour->clone();
		// Force motion_behaviour to have this milestone as parent and child
		this->motion_behaviour_->setChild(this);
		this->motion_behaviour_->setParent(this);
	}
	else
	{
		this->motion_behaviour_ = NULL;
	}

	// TODO (Roberto): This is old code from Lefteris. It sets the handle points of a Milestone (for ray-tracing) to be 4 points:
	// Two adding to the center point + or -RADIUS in x direction and two adding + or -RADIUS in y direction.
	Point tmp(configuration_[0],configuration_[1],0.0);
	tmp.x += RADIUS;
	handle_points_.push_back(tmp);
	tmp.x -= 2 * RADIUS;
	handle_points_.push_back(tmp);
	tmp.x += RADIUS;
	tmp.y += RADIUS;
	handle_points_.push_back(tmp);
	tmp.y -= 2 * RADIUS;
	handle_points_.push_back(tmp);
	//	if(this->motion_behaviour_)
	//	{
	//		this->motion_behaviour_->activate();
	//		double t = 0.0;
	//		for(int i = 0; i < 1000; ++i)
	//		{
	//			this->motion_behaviour_->update(t);
	//			t += 0.001;
	//		}
	//		::std::cout << " finished computing milestone" << ::std::endl;
	//		//this->motion_behaviour_->deactivate();
	//	}
}

OpSpaceMilestone::OpSpaceMilestone(TiXmlElement* milestone_xml, rxSystem* robot, double dt):
Milestone(),
motion_behaviour_(NULL)
//object_id_(-1)
{
	XMLDeserializer xml_deserializer_(milestone_xml);
	this->setStatus((Status)xml_deserializer_.deserializeInteger("status"));
	this->name_ = xml_deserializer_.deserializeString("name");
	//this->object_id_ = xml_deserializer_.deserializeInteger("ObjectId");
	this->posi_ori_selection_ = (PosiOriSelector)xml_deserializer_.deserializeInteger("PosiOriSelector");

	this->configuration_ = xml_deserializer_.deserializeVectorDouble("value");
	if(this->configuration_.size()==0)
	{
		throw std::string("ERROR: [OpSpaceMilestone::OpSpaceMilestone(TiXmlElement* milestone_xml, rxSystem* robot)] Child Element named \"Configuration\" not found in XML element milestone_xml.");
	}

	this->region_convergence_radius_ = xml_deserializer_.deserializeVectorDouble("epsilon");
	if(this->region_convergence_radius_.size() == 0)
	{
		throw std::string("ERROR: [OpSpaceMilestone::OpSpaceMilestone(TiXmlElement* milestone_xml, rxSystem* robot)] Child Element named \"RadiusRoC\" not found in XML element milestone_xml.");
	}

	TiXmlElement* handle_point_set_element = milestone_xml->FirstChildElement("HandlePoints");
	if(handle_point_set_element == NULL)
	{
		throw std::string("ERROR: [OpSpaceMilestone::OpSpaceMilestone(TiXmlElement* milestone_xml, rxSystem* robot)] Child Element named \"HandlePoints\" not found in XML element milestone_xml.");
	}

	for(TiXmlElement* handle_point_element = handle_point_set_element->FirstChildElement("HandlePoint"); handle_point_element; handle_point_element = handle_point_element->NextSiblingElement())
	{
		XMLDeserializer xml_deserializer_handle_point(handle_point_element);
		Point handle_point_value(-1.f,-1.f,-1.f);
		handle_point_value.x = xml_deserializer_handle_point.deserializeDouble("x");
		//std::cout << handle_point_value.x << std::endl;
		handle_point_value.y = xml_deserializer_handle_point.deserializeDouble("y");
		//std::cout << handle_point_value.y << std::endl;
		handle_point_value.z = xml_deserializer_handle_point.deserializeDouble("z");
		//std::cout << handle_point_value.z << std::endl;
		(this->handle_points_).push_back(handle_point_value);
	}

	TiXmlElement* mb_element = milestone_xml->FirstChildElement("MotionBehaviour");
	if(mb_element)
	{
		this->motion_behaviour_ = new MotionBehaviour(mb_element, this, this, robot, dt);
	}
}

OpSpaceMilestone::OpSpaceMilestone(const OpSpaceMilestone & op_milestone_cpy) :
Milestone(op_milestone_cpy),
configuration_(op_milestone_cpy.configuration_),
region_convergence_radius_(op_milestone_cpy.region_convergence_radius_),
//object_id_(op_milestone_cpy.object_id_),
handle_points_(op_milestone_cpy.handle_points_),
posi_ori_selection_(op_milestone_cpy.posi_ori_selection_)
{
	if(op_milestone_cpy.motion_behaviour_)
	{
		this->motion_behaviour_ = op_milestone_cpy.motion_behaviour_->clone();
		this->motion_behaviour_->setChild(this);
		this->motion_behaviour_->setParent(this);
	}
	else
	{
		this->motion_behaviour_ = NULL;
	}
}

OpSpaceMilestone::~OpSpaceMilestone()
{	
	// Do not delete anything but the MotionBehaviour, it is the only thing created/copied in the constructor
	delete motion_behaviour_;
	motion_behaviour_ = NULL;
}

void OpSpaceMilestone::setMotionBehaviour(MotionBehaviour * motion_behaviour)
{
	if(motion_behaviour)
	{
		this->motion_behaviour_ = motion_behaviour->clone();
		this->motion_behaviour_->setChild(this);
		this->motion_behaviour_->setParent(this);
	}
	else
	{
		this->motion_behaviour_ = NULL;
	}
}

void OpSpaceMilestone::setConfigurationSTDVector(std::vector<double> configuration_in, PosiOriSelector posi_ori_selector)
{
	configuration_.resize(6);
	int counter_offset = -1;
	int max_count = -1;
	switch(posi_ori_selector){
		case POSITION_SELECTION:
			counter_offset = 0;
			max_count = 3;
			switch(this->posi_ori_selection_){
		case ORIENTATION_SELECTION:
			this->posi_ori_selection_ = POS_AND_ORI_SELECTION;
			break;
		case POSITION_SELECTION:
		case POS_AND_ORI_SELECTION:
			break;
		default:
			this->posi_ori_selection_ = POSITION_SELECTION;
			break;
			}
			break;
		case ORIENTATION_SELECTION:
			switch(this->posi_ori_selection_){
		case POSITION_SELECTION:
			this->posi_ori_selection_ = POS_AND_ORI_SELECTION;
			break;
		case ORIENTATION_SELECTION:
		case POS_AND_ORI_SELECTION:
			break;
		default:
			this->posi_ori_selection_ = ORIENTATION_SELECTION;
			break;
			}
			counter_offset = 3;
			max_count = 6;
			break;
		case POS_AND_ORI_SELECTION:
			counter_offset = 0;
			max_count = 6;
			this->posi_ori_selection_ = POS_AND_ORI_SELECTION;
			break;
		default:
			throw std::string("ERROR [OpSpaceMilestone::setConfigurationSTDVector(std::vector<double> configuration_in, PosiOriSelector posi_ori_selector)]: Wrong posi_ori_selection value.");
			break;
	}
	int i = counter_offset;
	int n = 0;
	for(; i<max_count; i++, n++){
		configuration_[i] = configuration_in[n];
	}
}

std::vector<double> OpSpaceMilestone::getConfigurationSTDVector() const
{
	return this->configuration_;	
}

void OpSpaceMilestone::setConfiguration(dVector configuration_in, PosiOriSelector posi_ori_selector)
{
	configuration_.resize(6);
	int counter_offset = -1;
	int max_count = -1;
	switch(posi_ori_selector){
		case POSITION_SELECTION:
			counter_offset = 0;
			max_count = 3;
			switch(this->posi_ori_selection_){
		case ORIENTATION_SELECTION:
			this->posi_ori_selection_ = POS_AND_ORI_SELECTION;
			break;
		case POSITION_SELECTION:
		case POS_AND_ORI_SELECTION:
			break;
		default:
			this->posi_ori_selection_ = POSITION_SELECTION;
			break;
			}
			break;
		case ORIENTATION_SELECTION:
			switch(this->posi_ori_selection_){
		case POSITION_SELECTION:
			this->posi_ori_selection_ = POS_AND_ORI_SELECTION;
			break;
		case ORIENTATION_SELECTION:
		case POS_AND_ORI_SELECTION:
			break;
		default:
			this->posi_ori_selection_ = ORIENTATION_SELECTION;
			break;
			}
			counter_offset = 3;
			max_count = 6;
			break;
		case POS_AND_ORI_SELECTION:
			counter_offset = 0;
			max_count = 6;
			this->posi_ori_selection_ = POS_AND_ORI_SELECTION;
			break;
		default:
			throw std::string("ERROR [OpSpaceMilestone::setConfigurationSTDVector(std::vector<double> configuration_in, PosiOriSelector posi_ori_selector)]: Wrong posi_ori_selection value.");
			break;
	}
	int i = counter_offset;
	int n = 0;
	for(; i<max_count; i++, n++){
		configuration_[i] = configuration_in[n];
	}
}

dVector OpSpaceMilestone::getConfiguration() const
{
	// Using 6 values for the configuration: 3 position + 3 orientation
	dVector ret_value(6);
	ret_value.all(0);
	for(int i = 0; i<6; i++)
	{
		ret_value[i] = configuration_[i];
	}
	return ret_value;
}

//int OpSpaceMilestone::getObjectId() const
//{
//	return this->object_id_;
//}

void OpSpaceMilestone::update()
{
	if(motion_behaviour_ != NULL){
		cout << "CSM> Updating OpS Milestone" << endl;
	}else{
		cout << "CSM> Can't update anything, motion_behaviour_ is NULL!!!" << endl;
	}
}

std::string OpSpaceMilestone::toStringXML() const
{
	TiXmlDocument document_xml;
	TiXmlElement * node_element = this->toElementXML();
	document_xml.LinkEndChild(node_element);

	// Declare a printer
	TiXmlPrinter printer_xml;
	// attach it to the document you want to convert in to a std::string
	document_xml.Accept(&printer_xml);
	// Create a std::string and copy your document data in to the string
	std::string return_value = printer_xml.CStr();

	//TODO: Memory leaks?????????
	delete node_element;
	return return_value;
}

TiXmlElement* OpSpaceMilestone::toElementXML() const 
{
	TiXmlElement* op_space_ms_xml = new TiXmlElement("Milestone");
	op_space_ms_xml->SetAttribute("type", "OpSpace");
	op_space_ms_xml->SetAttribute("status", this->getStatus());
	//op_space_ms_xml->SetAttribute("ObjectId", object_id_);
	op_space_ms_xml->SetAttribute("PosiOriSelector", posi_ori_selection_);

	std::stringstream value_ss;
	for(int i=0; i<this->configuration_.size()-1; i++)
	{
		value_ss << configuration_.at(i) << " ";
	}
	value_ss << configuration_.at(configuration_.size()-1);
	op_space_ms_xml->SetAttribute("value", value_ss.str().c_str());

	std::stringstream epsilon_ss;
	for(int i=0; i<this->region_convergence_radius_.size()-1; i++)
	{
		epsilon_ss << region_convergence_radius_.at(i) << " ";
	}
	epsilon_ss << region_convergence_radius_.at(region_convergence_radius_.size()-1);
	op_space_ms_xml->SetAttribute("epsilon", epsilon_ss.str().c_str());

	TiXmlElement * handlePoints_txe = new TiXmlElement("HandlePoints");
	op_space_ms_xml->LinkEndChild(handlePoints_txe);
	std::vector<Point>::const_iterator handlePoints_it = handle_points_.begin();
	for (; handlePoints_it
		!= handle_points_.end(); handlePoints_it++) {
			TiXmlElement * handlePoint_txe;
			handlePoint_txe = new TiXmlElement("HandlePoint");
			handlePoints_txe->LinkEndChild(handlePoint_txe);
			handlePoint_txe->SetDoubleAttribute("x", handlePoints_it->x);
			handlePoint_txe->SetDoubleAttribute("y", handlePoints_it->y);
			handlePoint_txe->SetDoubleAttribute("z", handlePoints_it->z);
	}

	if(motion_behaviour_){
		op_space_ms_xml->LinkEndChild(this->motion_behaviour_->toElementXML());
	}
	return op_space_ms_xml;
}

OpSpaceMilestone* OpSpaceMilestone::clone() const
{
	OpSpaceMilestone* new_op_space_milestone = new OpSpaceMilestone(*this);
	return new_op_space_milestone;
}

//bool OpSpaceMilestone::operator ==(const OpSpaceMilestone & n) const
//{
//	return (this->name_ == n.getName());
//	//return (this->configuration_ == n.getConfigurationSTDVector() 
//	//	&& posi_ori_selection_ == n.posi_ori_selection_ && this->handle_points_ == n.handle_points_ 
//	//	&& this->object_id_ == n.getObjectId());
//}
//
//bool OpSpaceMilestone::operator ==(const Milestone & n) const
//{
//	const OpSpaceMilestone* n_opspace = dynamic_cast<const OpSpaceMilestone*>(&n);
//	if(n_opspace)
//	{
//		//return (this->configuration_ == n_opspace->getConfigurationSTDVector() 
//		//&& this->posi_ori_selection_ == n_opspace->posi_ori_selection_ && this->handle_points_ == n_opspace->handle_points_ 
//		//&& this->object_id_ == n_opspace->getObjectId());
//		return (this->name_ == n_opspace->getName());
//	}
//	else
//	{
//		return false;
//	}
//}
//
//bool OpSpaceMilestone::operator !=(const OpSpaceMilestone & n) const
//{
//	return !(*this==n);
//}
//
//bool OpSpaceMilestone::operator !=(const Milestone & n) const
//{
//	return !(*this==n);
//}

OpSpaceMilestone& OpSpaceMilestone::operator=(const OpSpaceMilestone & op_milestone_assignment)
{
	Milestone::operator=(op_milestone_assignment);
	this->configuration_ = op_milestone_assignment.getConfigurationSTDVector();
	if(op_milestone_assignment.motion_behaviour_)
	{
		this->motion_behaviour_ = op_milestone_assignment.motion_behaviour_->clone();
	}
	else
	{
		this->motion_behaviour_ = NULL;
	}
	this->region_convergence_radius_ = op_milestone_assignment.region_convergence_radius_;
	//this->object_id_ = op_milestone_assignment.getObjectId();
	this->handle_points_ = op_milestone_assignment.handle_points_;
	this->posi_ori_selection_ = op_milestone_assignment.getPosiOriSelector();
	return *this;
}

PosiOriSelector OpSpaceMilestone::getPosiOriSelector() const
{
	return posi_ori_selection_;
}

//bool OpSpaceMilestone::operator ==(const Milestone * n) const{
//	const OpSpaceMilestone* n_cspace = dynamic_cast<const OpSpaceMilestone*>(n);
//	if(n_cspace)
//		return (configuration_ == n_cspace->configuration_);
//	else
//		return false;
//}
//bool OpSpaceMilestone::operator ==(const OpSpaceMilestone * n) const{
//	return (configuration_ == n->configuration_);
//}
//
//bool OpSpaceMilestone::operator ==(OpSpaceMilestone * n) const{
//	return (configuration_ == n->configuration_);
//}