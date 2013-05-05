//
//  Behaviour.cpp
//  behaviour
//
//  Created by HengHong on 2/4/13.
//  Copyright (c) 2013 HengHong. All rights reserved.
//
//TODO : to be done!
#include "Behaviour.h"
extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdio>
#include <cstring>

#include <lua.h>

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
    lua_State* L;
    int luaadd ( int x, int y )
    {
        int sum;
        
        /* the function name */
        lua_getglobal(L, "add");
        
        /* the first argument */
        lua_pushnumber(L, x);
        
        /* the second argument */
        lua_pushnumber(L, y);
        
        /* call the function with 2 arguments, return 1 result */
        lua_call(L, 2, 1);
        
        /* get the result */
        sum = (int)lua_tointeger(L, -1);
        lua_pop(L, 1);
        
        return sum;
    }
    virtual Status update(){
        return BH_SUCCESS;
    };
    virtual void onInitialize(){
    }
    virtual void onTerminate(Status) {
        
        int sum;
        L = luaL_newstate();
        luaL_openlibs(L);
        
        /* load Lua base libraries */
        luaL_openlibs(L);
        
        /* load the script */
        luaL_dofile(L, func_name.c_str());
        
        /* call the add function */
        sum = luaadd( 10, 15 );
        
        /* print the result */
        printf( "The sum is %d\n", sum );
        
        /* cleanup Lua */
        lua_close(L);
        
        
        
        //m_eStatus = foo(arg);
        // this is for testing of the json , to input return values. normally return status should be calculated by foo
        m_eStatus = retStatus;
    }
    //TODO : Change this
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
    std::string func_name;
    std::string arg;
    Status retStatus;
    
private:
    Status m_eStatus;
    
    
};
//TODO : Edit this
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
