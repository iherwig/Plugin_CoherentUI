#include <StdAfx.h>
#include <Nodes/G2FlowBaseNode.h>
#include <CPluginCoherentUI.h>
#include <Coherent/UI/View.h>
#include "CoherentUISystem.h"

#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

using namespace rapidjson;

namespace CoherentUIPlugin
{
    class CFlowCUISendTokens : public CFlowBaseNode<eNCT_Instanced>
    {
        private:

            enum EInputPorts
            {
                EIP_ACTIVATE = 0,
                EIP_VIEWID,
            };

        public:
            CFlowCUISendTokens( SActivationInfo* pActInfo )
            {
            }

            virtual ~CFlowCUISendTokens()
            {
            }

            virtual IFlowNodePtr Clone( SActivationInfo* pActInfo )
            {
                return new CFlowCUISendTokens( pActInfo );
            }

            virtual void GetMemoryUsage( ICrySizer* s ) const
            {
                s->Add( *this );
            }

            void Serialize( SActivationInfo* pActInfo, TSerialize ser )
            {
            }

            virtual void GetConfiguration( SFlowNodeConfig& config )
            {
                static const SInputPortConfig inputs[] =
                {
                    InputPortConfig_Void( "Activate",                            _HELP( "activate view" ) ),
                    InputPortConfig<int>( "ViewID",                   0,         _HELP( "view id" ) ),
                    InputPortConfig_Null(),
                };

                config.pInputPorts = inputs;
                config.pOutputPorts = NULL;//output
                config.sDescription = _HELP( PLUGIN_CONSOLE_PREFIX "CoherentUI event trigger sending game tokens" );

                //config.nFlags |= EFLN_TARGET_ENTITY;
                config.SetCategory( EFLN_APPROVED );
            }

            virtual void ProcessEvent( EFlowEvent evt, SActivationInfo* pActInfo )
            {
                switch ( evt )
                {
                    case eFE_Suspend:
                        break;

                    case eFE_Resume:
                        break;

                    case eFE_Initialize:
                        break;

                    case eFE_Activate: 
                        {
                            int viewId = GetPortInt( pActInfo, EIP_VIEWID );
                            Coherent::UI::View* pView = gCoherentUISystem->GetView( viewId );
                            if ( pView )
                            {
                                StringBuffer buffer;
                                Writer<StringBuffer> writer(buffer);
                                writer.StartObject();

                                IGameTokenSystem *pGameTokenSystem = gEnv->pGame->GetIGameFramework()->GetIGameTokenSystem();
                                if(pGameTokenSystem)
                                {
                                    IGameTokenIt* pIt = pGameTokenSystem->GetGameTokenIterator();
                                    while(IGameToken *pGameToken=pIt->Next())
                                    {
                                        writer.String(pGameToken->GetName());
                                        writer.String(pGameToken->GetValueAsString());
                                    }
                                }
                                writer.EndObject();

                                pView->TriggerEvent( "GameTokens", buffer.GetString() );
                            }
                        }
                        break;

                    case eFE_Update:
                        break;
                }
            }
    };
}

REGISTER_FLOW_NODE_EX( "CoherentUI_Plugin:SendTokens", CoherentUIPlugin::CFlowCUISendTokens, CFlowCUISendTokens );