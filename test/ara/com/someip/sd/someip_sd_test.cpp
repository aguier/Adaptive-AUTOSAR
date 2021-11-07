#include <gtest/gtest.h>
#include "../../../../../src/ara/com/someip/sd/someip_sd_server.h"
#include "../../../../../src/ara/com/someip/sd/someip_sd_client.h"
#include "../../../../../src/ara/com/someip/sd/someip_sd_message.h"
#include "../../helper/mockup_network_layer.h"

namespace ara
{
    namespace com
    {
        namespace someip
        {
            namespace sd
            {
                class SomeIpSdTest : public testing::Test
                {
                private:
                    static const uint16_t cServiceId = 1;
                    static const uint16_t cInstanceId = 1;
                    static const uint8_t cMajorVersion = 1;
                    static const uint32_t cMinorVersion = 0;
                    static const uint16_t cPort = 8080;
                    static const int cInitialDelayMin = 100;

                    helper::MockupNetworkLayer<SomeIpSdMessage> mNetworkLayer;
                    helper::Ipv4Address mLocalhost;

                protected:
                    static const int cInitialDelayMax = 200;
                    static const int cRepetitionBaseDelay = 200;
                    static const uint32_t cRepetitionMax = 2;
                    static const int cCycleOfferDelay = 100;

                    SomeIpSdServer Server;
                    SomeIpSdClient Client;

                    SomeIpSdTest() : mNetworkLayer(),
                                     mLocalhost(127, 0, 0, 1),
                                     Server(
                                         &mNetworkLayer,
                                         cServiceId,
                                         cInstanceId,
                                         cMajorVersion,
                                         cMinorVersion,
                                         mLocalhost,
                                         cPort,
                                         cInitialDelayMin,
                                         cInitialDelayMax,
                                         cRepetitionBaseDelay,
                                         cCycleOfferDelay,
                                         cRepetitionMax),
                                     Client(
                                         &mNetworkLayer,
                                         cServiceId,
                                         cInitialDelayMin,
                                         cInitialDelayMax,
                                         cRepetitionBaseDelay,
                                         cRepetitionMax)
                    {
                    }
                };

                TEST_F(SomeIpSdTest, ServerStart)
                {
                    const helper::SdServerState cServerStoppedState =
                        helper::SdServerState::NotReady;

                    EXPECT_EQ(Server.GetState(), cServerStoppedState);
                    EXPECT_NO_THROW(Server.Start());
                    EXPECT_THROW(Server.Start(), std::logic_error);
                    EXPECT_NO_THROW(Server.Stop());
                }

                TEST_F(SomeIpSdTest, ClientStart)
                {
                    const helper::SdClientState cClientStoppedState =
                        helper::SdClientState::ServiceNotSeen;

                    EXPECT_EQ(Client.GetState(), cClientStoppedState);
                    EXPECT_NO_THROW(Client.Start());
                    EXPECT_THROW(Client.Start(), std::logic_error);
                    EXPECT_NO_THROW(Client.Stop());
                }

                TEST_F(SomeIpSdTest, StartScenario)
                {
                    const helper::SdClientState cServiceReadyState =
                        helper::SdClientState::ServiceReady;

                    int _duration =
                        // Initial wait phase delay
                        (cInitialDelayMax +
                         // Summation of all the repetition phase delays
                         cRepetitionBaseDelay * (std::pow(2, cRepetitionMax) - 1) +
                         // Main main first cycle delay
                         cCycleOfferDelay) *
                        // Apply minimum Nyquist–Shannon margin (make the duration twice longer)
                        2;

                    Server.Start();
                    Client.Start();
                    Client.TryWaitUntiServiceOffered(_duration);

                    EXPECT_EQ(Client.GetState(), cServiceReadyState);
                }
            }
        }
    }
}