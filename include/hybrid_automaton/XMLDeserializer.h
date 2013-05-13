#ifndef XML_DESERIALIZER
#define XML_DESERIALIZER

#include <string>
#include <map>
#include "tinyxml.h"

#include "HybridAutomaton.h"
#include "MotionBehaviour.h"
#include "Milestone.h"
#include "CSpaceMilestone.h"
#include "CSpaceBlackBoardMilestone.h"
#include "OpSpaceMilestone.h"
#include "PostureMilestone.h"


template<class T>
T deserializeElement(TiXmlElement * xml_element, const char * field_name);

template<class T>
T deserializeElement(TiXmlElement * xml_element, const char * field_name, T default_value);

bool deserializeBoolean(TiXmlElement * xml_element, const char * field_name, bool default_value = false);

std::string deserializeString(TiXmlElement * xml_element, const char * field_name, const std::string& default_value);
std::string deserializeString(TiXmlElement * xml_element, const char * field_name, bool error_if_not_found=true);

dVector deserializeDVector(TiXmlElement * xml_element, const char * field_name);

Displacement deserializeDisplacement(TiXmlElement * xml_element, const char * field_name, const Displacement& default_value);
Rotation deserializeRotation(TiXmlElement * xml_element, const char * field_name, const Rotation& default_value);

rxBody* deserializeBody(rxSystem* robot, TiXmlElement * xml_element, const char * field_name, rxBody* default_value);

template<class T>
std::vector<T> deserializeStdVector(TiXmlElement * xml_element, const char * field_name);

ViaPointBase * deserializeViaPoint(TiXmlElement * xml_element, ControllerType type_of_controller, int controller_dimension);

/**
* Replace the colons of a string with white spaces.
* @param text String to be processed.
*/
std::string colon2space(std::string text);

struct ControllerParameters {
	std::string type;
	bool ik;
	dVector kp;
	dVector kv;
	dVector invL2sqr;
	dVector stiffness_b;
	dVector stiffness_k;
	double impedance_m;
	double impedance_b;
	double impedance_k;
	rxBody* alpha;
	rxBody* beta;
	Displacement alpha_displacement;
	Displacement beta_displacement;
	Rotation alpha_rotation_matrix;
	Rotation beta_rotation_matrix;
	std::string blackboard_variable_name;
	double maxVel;
	int priority;
	double timeGoal;
	bool reuseGoal;
	int typeGoal;
	dVector dVectorGoal;
	Vector3D Vector3DGoal;
	Rotation RGoal;
	Displacement rGoal;
	double desired_distance;
	double max_force;
	double max_vel;
	std::string limit_body;
	double distance_limit;
	double distance_threshold;
	double deactivation_threshold;
	std::vector<long> index;
	double safetyThresh;
	std::vector<double> tc;
};

class XMLDeserializer
{
public:
	/**
	* Convert a wstring into a string.
	* @param wstr Wide string to be converted.
	*/
	static std::string wstring2string(const std::wstring& wstr);

	/**
	* Convert a string into a wstring.
	* @param str String to be converted.
	*/
	static std::wstring string2wstring(const std::string& str);

	static Rotation string2rotation(const std::string& str, const Rotation& default_value);

	static void addPoint(rxController* controller, TiXmlElement* xml);

	XMLDeserializer();
	virtual ~XMLDeserializer();

	static HybridAutomaton* createHybridAutomaton(const std::string& xml_string, rxSystem* robot, double dT);

	static CSpaceMilestone* createCSpaceMilestone(TiXmlElement* milestone_xml, rxSystem* robot, double dT);
	static CSpaceBlackBoardMilestone* createCSpaceBlackBoardMilestone(TiXmlElement* milestone_xml, rxSystem* robot, double dT);

	static OpSpaceMilestone* createOpSpaceMilestone(TiXmlElement* milestone_xml, rxSystem* robot, double dT);
	
	static PostureMilestone* createPostureMilestone(TiXmlElement* milestone_xml, rxSystem* robot, double dT);
	
	static MotionBehaviour* createMotionBehaviour(TiXmlElement* motion_behaviour_xml , Milestone *dad, Milestone *son , rxSystem* robot, double dT );

	static rxController* createController(TiXmlElement* rxController_xml , const Milestone *dad, const Milestone *son , rxSystem* robot, double dT, bool goal_controller, int controller_counter);
	
	static std::map<std::string, ControllerType> createControllerMapping();

	static std::map<std::string, ControllerType>	controller_map_;	// Translation between class name (string) of the controllers and their type
};

#endif	//XML_DESERIALIZER