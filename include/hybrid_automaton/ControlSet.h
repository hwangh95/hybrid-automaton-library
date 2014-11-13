#ifndef HYBRID_AUTOMATON_CONTROLSET_H_
#define HYBRID_AUTOMATON_CONTROLSET_H_

#include "hybrid_automaton/Controller.h"
#include "hybrid_automaton/Serializable.h"

#include <boost/shared_ptr.hpp>

#include <Eigen/Dense>

#include <vector>

namespace ha {

	class ControlSet;
	typedef boost::shared_ptr<ControlSet> ControlSetPtr;

	class ControlSet : public Serializable {

	protected:
		//std::vector<Controller::Ptr> _controllers;

	public:
		typedef boost::shared_ptr<ControlSet> Ptr;

		ControlSet() {
		}

		virtual void activate() {
			throw "not implemented";
		}

		virtual void deactivate() {
			throw "not implemented";
		}

		virtual ::Eigen::VectorXd step(const double& t) {
			throw "not implemented"; 
		}

		virtual std::vector<Controller::Ptr> getControllers() {
			throw "not implemented";
		}

		virtual void serialize(DescriptionTreeNode::Ptr& tree) const;
		virtual void deserialize(const DescriptionTreeNode::Ptr tree);

		ControlSetPtr clone() const {
			return ControlSetPtr(_doClone());
		}

	protected:
		virtual ControlSet* _doClone() const {
			return new ControlSet(*this);
		}
	};

}

#endif
