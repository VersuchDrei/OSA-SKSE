#include <Core/Thread.h>
#include <Graph/Node.h>
#include <Messaging/IMessages.h>

namespace OStim {

    Thread::Thread(ThreadId a_id, std::vector<RE::Actor*> a_actors) {
        m_threadId = a_id;
        for (int i = 0; i < a_actors.size(); i++) {
            m_actors.insert(std::make_pair(i, a_actors[i]));
        }
    }

    void Thread::ChangeNode(Graph::Node* a_node)
    {
        m_currentNode = a_node;
        auto messaging = SKSE::GetMessagingInterface();
        Messaging::AnimationChangedMessage msg;
        msg.newAnimation = a_node;        
        logger::info("Sending animation changed event");
        Messaging::MessagingRegistry::GetSingleton()->SendMessageToListeners(msg);
        
    }

    void Thread::AddThirdActor(RE::Actor* a_actor) { m_actors.insert(std::make_pair(2, a_actor)); }

    void Thread::RemoveThirdActor() { m_actors.erase(2); }

}  // namespace OStim