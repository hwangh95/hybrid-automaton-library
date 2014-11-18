#ifndef HYBRID_AUTOMATON_CONTROL_SWITCH_H_
#define HYBRID_AUTOMATON_CONTROL_SWITCH_H_

#include <boost/shared_ptr.hpp>

#include <vector>

#include "hybrid_automaton/JumpCondition.h" 
#include "hybrid_automaton/ControlMode.h" 
#include "hybrid_automaton/Serializable.h"

namespace ha {

  class ControlSwitch;
  typedef boost::shared_ptr<ControlSwitch> ControlSwitchPtr;

  class ControlSwitch : public Serializable
  {
  public:
    typedef boost::shared_ptr<ControlSwitch> Ptr;

    ControlSwitch() {}

    virtual ~ControlSwitch() {}

    ControlSwitchPtr clone() const
    {
      return (ControlSwitchPtr(_doClone()));
    }

	virtual void activate(const double& t);
	virtual void deactivate();

	virtual void step(const double& t);
    virtual bool isActive() const;

	virtual void add(const JumpConditionPtr& jump_condition);
	virtual const std::vector<JumpConditionPtr>& getJumpConditions();

	virtual DescriptionTreeNode::Ptr serialize(const DescriptionTree::ConstPtr& factory) const;
	virtual void deserialize(const DescriptionTreeNode::ConstPtr& tree, const System::ConstPtr& system);

	virtual void setSourceControlModeName(const std::string& source);
	virtual const std::string getSourceControlModeName() const;
	virtual void setTargetControlModeName(const std::string& target);
	virtual const std::string getTargetControlModeName() const;

	virtual void setSourceControlMode(const ControlMode::ConstPtr& source);
	virtual ControlMode::ConstPtr getSourceControlMode() const;
	virtual void setTargetControlMode(const ControlMode::ConstPtr& target);
	virtual ControlMode::ConstPtr getTargetControlMode() const;

	virtual void setName(const std::string& name);
	virtual const std::string getName() const;

  protected:
    std::vector<JumpConditionPtr> _jump_conditions;

	std::string _source_control_mode_name;
	std::string _target_control_mode_name;

	ControlMode::ConstPtr _source_control_mode;
	ControlMode::ConstPtr _target_control_mode;

	std::string _name;

    virtual ControlSwitch* _doClone() const
    {
      return (new ControlSwitch(*this));
    }
  };

}

#endif
