// Copyright 2016 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*************************************************************************
 * @file FlowControlExampleSubscriber.cpp
 * This file contains the implementation of the subscriber functions.
 *
 * This file was generated by the tool fastcdrgen.
 */

#include <fastrtps/participant/Participant.h>
#include <fastrtps/attributes/ParticipantAttributes.h>
#include <fastrtps/subscriber/Subscriber.h>
#include <fastrtps/attributes/SubscriberAttributes.h>

#include <fastrtps/Domain.h>

#include "FlowControlExampleSubscriber.h"

using namespace eprosima::fastrtps;
using namespace eprosima::fastrtps::rtps;

FlowControlExampleSubscriber::FlowControlExampleSubscriber() : mp_participant(nullptr), mp_subscriber(nullptr) {}

FlowControlExampleSubscriber::~FlowControlExampleSubscriber() {	Domain::removeParticipant(mp_participant);}

bool FlowControlExampleSubscriber::init()
{
    // Create RTPSParticipant

    ParticipantAttributes PParam;
    PParam.rtps.builtin.domainId = 0; //MUST BE THE SAME AS IN THE PUBLISHER
    PParam.rtps.builtin.leaseDuration = c_TimeInfinite;
    PParam.rtps.setName("Participant_subscriber"); //You can put the name you want
    mp_participant = Domain::createParticipant(PParam);
    if(mp_participant == nullptr)
        return false;

    //Register the type

    Domain::registerType(mp_participant,(TopicDataType*) &myType);		

    // Create Subscriber

    SubscriberAttributes Rparam;
    Rparam.topic.topicKind = NO_KEY;
    Rparam.topic.topicDataType = myType.getName(); //Must be registered before the creation of the subscriber
    Rparam.topic.topicName = "FlowControlExamplePubSubTopic";
    mp_subscriber = Domain::createSubscriber(mp_participant,Rparam,(SubscriberListener*)&m_listener);
    if(mp_subscriber == nullptr)
        return false;
    return true;
}

void FlowControlExampleSubscriber::SubListener::onSubscriptionMatched(Subscriber*, MatchingInfo& info)
{
    if (info.status == MATCHED_MATCHING)
    {
        n_matched++;
        std::cout << "Subscriber matched" << std::endl;
    }
    else
    {
        n_matched--;
        std::cout << "Subscriber unmatched" << std::endl;
    }
}

void FlowControlExampleSubscriber::SubListener::onNewDataMessage(Subscriber* sub)
{
    // Take data
    FlowControlExample st;

    if(sub->takeNextData(&st, &m_info))
    {
        if(m_info.sampleKind == ALIVE)
        {
            ++n_msg;
            static unsigned int fastMessages = 0;
            static unsigned int slowMessages = 0;
            // Print your structure data here.
            if (st.wasFast())
            {
                fastMessages++;
                std::cout << "Sample received from fast writer, count=" << fastMessages << std::endl;
            }
            else
            {
                slowMessages++;
                std::cout << "Sample received from slow writer, count=" << slowMessages << std::endl;
            }
        }
    }
}

void FlowControlExampleSubscriber::run()
{
    std::cout << "Waiting for Data, press Enter to stop the Subscriber. "<<std::endl;
    std::cin.ignore();
    std::cout << "Shutting down the Subscriber." << std::endl;
}

