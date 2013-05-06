//
//  main.cpp
//  BehaviourTree
//
//  Created by HengHong on 27/3/13.
//  Copyright (c) 2013 HengHong. All rights reserved.
//
// TODO : More includes here
#include "Behaviour.h"
#include "Behaviour.cpp"
#include <json/json.h>
#include <lib_json/json_reader.cpp>
#include <lib_json/json_value.cpp>
#include <lib_json/json_writer.cpp>
using namespace std;







Behavior* buildTreeWithJsonValue(Json::Value json)
{
    //find out if its a behaviour/sequence/selector
    if (json["type"]=="selector")
    {
    
        Selector* root = new Selector();
        for (int i = 0; i<json["items"].size(); i++) {
            root->m_Children.push_back(buildTreeWithJsonValue(json["items"].operator[](i)));
        }
        return root;
        
        
    }else if (json["type"]=="sequence")
    {
        Sequence* rootseq = new Sequence();
        for (int i = 0; i<json["items"].size(); i++) {
            rootseq->m_Children.push_back(buildTreeWithJsonValue(json["items"].operator[](i)));
        }
        return rootseq;
        
    }else
    {
        // ** Map string name to function names, add more statements when adding more functions ** //

            Behavior* b = new Behavior();
            b->func_name = json["func_name"].asString();
            b->name = json["name"].asString();
            b->arg = json["arg"].asString();
            if (json["return"]=="success") {
                b->retStatus = BH_SUCCESS;
            }else{
                b->retStatus = BH_FAILURE;
            }
            return b;
        
        
    }
    

    
}


static int printMessage (lua_State *lua)
{
    assert (lua_isstring (lua,1));
    
    const char *msg = lua_tostring (lua, 1);
    
    // debug output
    cout << "script: " << msg << endl;
    return 0;
}


int main(int argc, const char * argv[])
{
    string config_doc = "{\"name\":\"root\",\"type\":\"selector\",\"items\":[{\"name\":\"hashouse?\",\"type\":\"selector\",\"items\":[{\"name\":\"Enough Money For New House?\",\"type\":\"selector\",\"items\":[{\"name\":\"Yes\",\"type\":\"sequence\",\"items\":[{\"name\":\"isHouseNearBy?\",\"type\":\"behaviour\",\"func_name\":\"foo.lua\",\"arg\":\"New house nearby!\",\"return\":\"success\"},{\"name\":\"BuyHouse\",\"type\":\"behaviour\",\"func_name\":\"foo.lua\",\"arg\":\"Buying House...\",\"return\":\"success\"},{\"name\":\"Move In\",\"type\":\"behaviour\",\"func_name\":\"foo.lua\",\"arg\":\"Moving into new house...\",\"return\":\"success\"}]},{\"name\":\"No\",\"type\":\"behaviour\",\"func_name\":\"foo.lua\",\"arg\":\"Not enough money for house!\",\"return\":\"success\"}]},{\"name\":\"Enough Money For New House?\",\"type\":\"sequence\",\"items\":[{\"name\":\"isHouseNearBy?\",\"type\":\"behaviour\",\"func_name\":\"foo.lua\",\"arg\":\"New house nearby!\",\"return\":\"success\"},{\"name\":\"BuyHouse\",\"type\":\"behaviour\",\"func_name\":\"foo.lua\",\"arg\":\"Buying House...\",\"return\":\"success\"},{\"name\":\"Move In\",\"type\":\"behaviour\",\"func_name\":\"foo.lua\",\"arg\":\"Moving into new house...\",\"return\":\"success\"}]}]},{\"name\":\"Food\",\"type\":\"selector\",\"items\":[{\"name\":\"Hungry?\",\"type\":\"selector\",\"items\":[{\"name\":\"Yes\",\"type\":\"sequence\",\"items\":[{\"name\":\"is there food nearby?\",\"type\":\"selector\",\"items\":[{\"name\":\"Go To Shop\",\"type\":\"behaviour\",\"func_name\":\"foo.lua\",\"arg\":\"Moving to shop...\",\"return\":\"success\"},{\"name\":\"Go To Shop\",\"type\":\"behaviour\",\"func_name\":\"foo.lua\",\"arg\":\"Moving to shop...\",\"return\":\"success\"}]},{\"name\":\"Go To Shop\",\"type\":\"behaviour\",\"func_name\":\"foo.lua\",\"arg\":\"Moving to shop...\",\"return\":\"success\"},{\"name\":\"Buy Food\",\"type\":\"behaviour\",\"func_name\":\"foo.lua\",\"arg\":\"Buying food at shop\",\"return\":\"success\"},{\"name\":\"Eat\",\"type\":\"behaviour\",\"func_name\":\"foo.lua\",\"arg\":\"Eating...\",\"return\":\"success\"}]},{\"name\":\"No\",\"type\":\"behaviour\",\"func_name\":\"foo.lua\",\"arg\":\"Not Hungry\",\"return\":\"success\"}]}]},{\"name\":\"Work\",\"type\":\"selector\",\"items\":[{\"name\":\"Hungry?\",\"type\":\"behaviour\",\"func_name\":\"foo.lua\",\"arg\":\"asd\",\"return\":\"success\"}]}]}";
    Json::Value root;   // will contains the root value after parsing.
    Json::Reader reader;
    bool parsingSuccessful = reader.parse( config_doc, root );
    if ( !parsingSuccessful )
    {
        std::cout  << "Failed to parse configuration\n" << reader.getFormatedErrorMessages();
        return 0;
    }
    std::string encoding = root.get("encoding", "UTF-8" ).asString();


    Behavior* behavTree = buildTreeWithJsonValue(root);
    
    std::cout<<"created tree\n\n";
    
    //always reinitialize the behavior tree to point the current child back to the first left child
    behavTree->onInitialize();
    
    // this is the main method that starts off all the calculations
    Status s = behavTree->update();
    
    if (s == BH_SUCCESS) {
        std::cout << "success!\n";
    }
    return 0;

}

