#include <iostream>

//The PUBLISH packet includes the Subscription Identifier carried in the SUBSCRIBE packet, if there was one.

//ONLY PACKETS PUSHED FROM SERVER TO RECIEVING CLIENTS REQUIRE A SUBSCRIPTION IDENTIFIER...

//If a Client receives an unsolicited Application Message(not resulting from a subscription) which has a QoS greater than Maximum QoS, it uses a DISCONNECT packet with Reason Code 0x9B (QoS not supported).
//=================================================================================================================================================

//===============================================CLIENT=====================================================================================================================================================
//The action of the recipient when it receives a PUBLISH packet depends on the QoS level as described in section 4.3.

//If the PUBLISH packet contains a Topic Alias, the receiver processes it as follows :
//   A Topic Alias value of 0 or greater than the Maximum Topic Alias is a Protocol Error, the receiver uses DISCONNECT with Reason Code of 0x94 (Topic Alias invalid) as described in section 4.13.
// If the receiver has already established a mapping for the Topic Alias, then
// If the packet has a zero length Topic Name, the receiver processes it using the Topic Name that corresponds to the Topic Alias
// If the packet contains a non - zero length Topic Name, the receiver processes the packet using that Topic Name and updates its mapping for the Topic Alias to the Topic Name from the incoming packet
// If the receiver does not already have a mapping for this Topic Alias
// If the packet has a zero length Topic Name field it is a Protocol Error and the receiver uses DISCONNECT with Reason Code of 0x82 (Protocol Error) as described in section 4.13.
// If the packet contains a Topic Name with a non - zero length, the receiver processes the packet using that Topic Name and sets its mappings for the Topic Alias to Topic Name from the incoming packet.
//============================================================================================================================================================================================================

//===================================================================SUBSCRIBE=ACTIONS==============================================================================================================

//==========================================================================SUBSCRIBE=QOS=============================================================================================================

//A publisher determines the maximum QoS a Message can be delivered at, but a subscriber can ask the Server to downgrades the QoS to one more suitable for its usage.
// 
// ==========================================================================SUBSCRIPTION=IDENTIFIERS==================================================================================================
// 
// 
//The Subscription Identifiers do not form part of the Client’s Session State in the Client.In a useful implementation, a Client will associate the Subscription Identifiers with other Client side state, this state is 
// typically removed when the Client unsubscribes, when the Client subscribes for the same Topic Filter with a different identifier or no identifier, or when the Client receives Session Present 0 in a CONNACK packet.
// 
// The Server need not use the same set of Subscription Identifiers in the retransmitted PUBLISH packet The Client can remake a Subscription by sending a SUBSCRIBE packet containing a Topic Filter that is
//identical to the Topic Filter of an existing Subscription in the current Session.

//Usage scenarios, for illustration of Subscription Identifiers.
//The Client implementation indicates via its programming interface that a publication matched more than one subscription.
// The Client implementation generates a new identifier each time a subscription is made.
//If the returned publication carries more than one Subscription Identifier, then the publication matched more than one subscription.
//The Client implementation allows the subscriber to direct messages to a callback associated with the subscription.
//The Client implementation generates an identifier which uniquely maps the identifier to the callback
// When a publication is received it uses the Subscription Identifier to determine which callback is driven.
//The Client implementation returns the topic string used to make the subscription to the application when it delivers the published message.
// To achieve this the Client generates an identifier which uniquely identifies the Topic Filter.
// When a publication is received the Client implementation uses the identifiers to look up the original Topic Filters and return them to the Client application.
// A gateway forwards publications received from a Server to Clients that have made subscriptions to the gateway.
// The gateway implementation maintains a map of each unique Topic Filter it receives to the set of ClientID, Subscription Identifier pairs that it also received.
// It generates a unique identifier for each Topic Filter that it forwards to the Server.
// When a publication is received, the gateway uses the Subscription Identifiers it received from the Server to look up the Client Identifier, Subscription Identifier pairs associated with them.
// It adds these to the PUBLISH packets it sends to the Clients.If the upstream Server sent multiple PUBLISH packets because the message matched multiple subscriptions, then this behavior is mirrored to the Clients

//===================================================================DISCONNECT=ACTION===============================================================================================================================
//After sending a DISCONNECT packet the sender :

//MUST NOT send any more MQTT Control Packets on that Network Connection[MQTT - 3.14.4 - 1].
//MUST close the Network Connection[MQTT - 3.14.4 - 2].
//On receipt of DISCONNECT, the receiver :
//SHOULD close the Network Connection.










