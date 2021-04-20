#pragma once

#include "Common.h"
#include "Entity.h"

class Action
{
    std::string m_name  = "NONE"; 
    std::string m_type  = "NONE"; 
    
public:

    Action();
    Action(const std::string & name, const std::string & type);

    const std::string & name() const;
    const std::string & type() const;
    std::string toString() const;
};
