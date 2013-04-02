//
//  main.cpp
//  BehaviourTree
//
//  Created by HengHong on 27/3/13.
//  Copyright (c) 2013 HengHong. All rights reserved.
//
#include <iostream>
#include <vector>
#include <cstdio>
#include <cstring>
#include <json/json.h>
#include <lib_json/json_reader.cpp>
#include <lib_json/json_value.cpp>
#include <lib_json/json_writer.cpp>

using namespace std;


enum Status
{
    BH_INVALID,
    BH_SUCCESS,
    BH_FAILURE,
    BH_RUNNING,
};


class Functions
{
public:
    static Status print_func(std::string x)
    {
        std::cout << x << endl;
        return BH_SUCCESS;
    }
    // ** Implement your own functions here to detect game metrics and variables and decide if a behavior/action is successful ** //
    
};

class Behavior
{
public:
    virtual Status update(){
        return BH_SUCCESS;
    };
    virtual void onInitialize(){
    }
    virtual void onTerminate(Status) {
            m_eStatus = foo(arg);
            // this is for testing of the json , to input return values. normally return status should be calculated by foo
            m_eStatus = retStatus;
    }

    Behavior(){};
    
    Behavior(Status (*food)(std::string)){
        foo = food;
    };
    
    Status tick()
    {
        if (m_eStatus == BH_INVALID)
        {
            onInitialize();
        }
        m_eStatus = update();
        if (m_eStatus != BH_RUNNING)
        {
            onTerminate(m_eStatus);
        }
        return m_eStatus;
    }
    
    Status (*foo)(std::string);
    std::string name;
    std::string arg;
    Status retStatus;
    
private:
    Status m_eStatus;

    
};
class Composite : public Behavior
{
public:
    typedef std::vector <Behavior*> Behaviors;
    Behaviors m_Children;
    virtual void onTerminate(Status) {
    }
};

class Sequence : public Composite
{
public:
     
    virtual void onInitialize(){
        m_CurrentChild = m_Children.begin();
    }
    virtual Status update()
    {
        
        while (true)
        {
            Status s = (*m_CurrentChild)->tick();
            if (s != BH_SUCCESS) {
                return s;
            }
            
            // Hit the end of the array, job done!
            if(++m_CurrentChild == m_Children.end())
            {
                return BH_SUCCESS;
            }
        }
        return BH_INVALID;
    }
    
    Behaviors::iterator m_CurrentChild;
};


class Selector : public Composite
{
public:
    virtual void onInitialize(){
        m_CurrentChild = m_Children.begin();
    }
    virtual Status update()
    {
        while (true)
        {
            Status s = (*m_CurrentChild)->tick();
            
            if (s != BH_FAILURE) {
                
                return s;
            }
            
            if(++m_CurrentChild == m_Children.end())
            {
                return BH_FAILURE;
            }
        }
        return BH_INVALID;
    }
    
    Behaviors::iterator m_CurrentChild;
    
};



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
        if (json["func_name"] == "print_func") {
            Behavior* b = new Behavior(&(Functions::print_func));
            b->name = json["name"].toStyledString();
            b->arg = json["arg"].toStyledString();
            if (json["return"]=="success") {
                b->retStatus = BH_SUCCESS;
            }else{
                b->retStatus = BH_FAILURE;
            }
            return b;
        }else{
            Behavior* b = new Behavior();
            return b;
        }
        
        
        
        
    }
    

    
}





int main(int argc, const char * argv[])
{
    
    string config_doc = "{\"name\":\"root\",\"type\":\"selector\",\"items\":[{\"name\":\"hashouse?\",\"type\":\"selector\",\"items\":[{\"name\":\"Enough Money For New House?\",\"type\":\"selector\",\"items\":[{\"name\":\"Yes\",\"type\":\"sequence\",\"items\":[{\"name\":\"isHouseNearBy?\",\"type\":\"behaviour\",\"func_name\":\"print_func\",\"arg\":\"New house nearby!\",\"return\":\"success\"},{\"name\":\"BuyHouse\",\"type\":\"behaviour\",\"func_name\":\"print_func\",\"arg\":\"Buying House...\",\"return\":\"success\"},{\"name\":\"Move In\",\"type\":\"behaviour\",\"func_name\":\"print_func\",\"arg\":\"Moving into new house...\",\"return\":\"success\"}]},{\"name\":\"No\",\"type\":\"behaviour\",\"func_name\":\"print_func\",\"arg\":\"Not enough money for house!\",\"return\":\"success\"}]},{\"name\":\"Enough Money For New House?\",\"type\":\"sequence\",\"items\":[{\"name\":\"isHouseNearBy?\",\"type\":\"behaviour\",\"func_name\":\"print_func\",\"arg\":\"New house nearby!\",\"return\":\"success\"},{\"name\":\"BuyHouse\",\"type\":\"behaviour\",\"func_name\":\"print_func\",\"arg\":\"Buying House...\",\"return\":\"success\"},{\"name\":\"Move In\",\"type\":\"behaviour\",\"func_name\":\"print_func\",\"arg\":\"Moving into new house...\",\"return\":\"success\"}]}]},{\"name\":\"Food\",\"type\":\"selector\",\"items\":[{\"name\":\"Hungry?\",\"type\":\"selector\",\"items\":[{\"name\":\"Yes\",\"type\":\"sequence\",\"items\":[{\"name\":\"is there food nearby?\",\"type\":\"selector\",\"items\":[{\"name\":\"Go To Shop\",\"type\":\"behaviour\",\"func_name\":\"print_func\",\"arg\":\"Moving to shop...\",\"return\":\"success\"},{\"name\":\"Go To Shop\",\"type\":\"behaviour\",\"func_name\":\"print_func\",\"arg\":\"Moving to shop...\",\"return\":\"success\"}]},{\"name\":\"Go To Shop\",\"type\":\"behaviour\",\"func_name\":\"print_func\",\"arg\":\"Moving to shop...\",\"return\":\"success\"},{\"name\":\"Buy Food\",\"type\":\"behaviour\",\"func_name\":\"print_func\",\"arg\":\"Buying food at shop\",\"return\":\"success\"},{\"name\":\"Eat\",\"type\":\"behaviour\",\"func_name\":\"print_func\",\"arg\":\"Eating...\",\"return\":\"success\"}]},{\"name\":\"No\",\"type\":\"behaviour\",\"func_name\":\"print_func\",\"arg\":\"Not Hungry\",\"return\":\"success\"}]}]},{\"name\":\"Work\",\"type\":\"selector\",\"items\":[{\"name\":\"Hungry?\",\"type\":\"behaviour\",\"func_name\":\"print_func\",\"arg\":\"asd\",\"return\":\"success\"}]}]}";
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

