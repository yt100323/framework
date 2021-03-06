/*
 * SimpleActionState.h
 *
 *  Created on: Mar 1, 2012
 *      Author: Peter Hohnloser
 */

#ifndef SIMPLEACTIONSTATE_H_
#define SIMPLEACTIONSTATE_H_
#include <sweeper_state_machine/State.h>
#include <actionlib/client/simple_action_client.h>
#include <actionlib/client/terminal_state.h>
#include "actionlib/client/simple_goal_state.h"
#include "actionlib/client/simple_client_goal_state.h"
#include <boost/thread.hpp>
#include <boost/function.hpp>

using std::string;

/* The class SimpleActionState is an abstract class which means
 *  that another class has to inherit from it.
 */

template<class ASpec, class AGoal>
  class SimpleActionState : public State
  {
  protected:
    ACTION_DEFINITION(ASpec);
    typedef actionlib::SimpleActionClient<ASpec> client_;
    AGoal goal_;
    client_ ac_;
    bool first_;
  public:
    typedef boost::function<void(const actionlib::SimpleClientGoalState& state, const ResultConstPtr& result)>
                                                                                                               SimpleDoneCallback;
    typedef boost::function<void(const FeedbackConstPtr& feedback)> SimpleFeedbackCallback;
    typedef boost::function<void()> SimpleActiveCallback;

    /*
     * Constructor needs the name of the SimpleActionServer node
     */

    SimpleActionState(string serverNode) :
      ac_(serverNode, true)
    {
      feedback_cb_ = SimpleFeedbackCallback();
      done_cb_ = SimpleDoneCallback();
      first_ = true;
      ROS_INFO("starting to wait for server %s",serverNode.c_str());
      ac_.waitForServer(ros::Duration(5));
            if (ac_.isServerConnected())
          	  ROS_INFO("connected to server %s",serverNode.c_str());
            else
                ROS_WARN("#GUI NOT!!! connected to server %s",serverNode.c_str());

    }

    virtual ~SimpleActionState()
    {
    }
    /**
     * sends the call to the SimpleActionServer
     */
    void sendGoal()
    {
      ac_.sendGoal(goal_, done_cb_, SimpleActiveCallback(), feedback_cb_);
      ac_.waitForResult(ros::Duration(0.1));
    }
    /**
     *
     */
    virtual bool setPause(bool p)
    {
      /*
      if (p)
      {
        if (!first_)
        {
          //ac_.cancelGoal();
        }
      }
      else
      {
        ac_.sendGoal(goal_, done_cb_, SimpleActiveCallback(), feedback_cb_);
        ac_.waitForResult(ros::Duration(0.1));
      }
      */
      mx.lock();
      pause = p;
      mx.unlock();

      return true;
    }
    /**
     *  sets the feedback callback method for the SimpleActionCleint
     */
    void setFeedbackCallback(SimpleFeedbackCallback feedback_cb)
    {
      feedback_cb_ = feedback_cb;
    }
    /*
     * set the done callback method for the SimpleActinClient
     */
    void setDoneCallback(SimpleDoneCallback done_cb)
    {
      done_cb_ = done_cb;
    }

    void stop(){
     if(!first_)
     {
    	 ac_.cancelGoal();
     }
      first_ = true;
    }

  private:
    SimpleDoneCallback done_cb_;
    SimpleFeedbackCallback feedback_cb_;
  };

#endif /* SIMPLEACTIONSTATE_H_ */
