#pragma once
#include <string>
#include <vector>
#include <map>

struct DialogChoice {
    std::string text;
    std::string nextNode;
    float suspicionDelta;
};

struct DialogNode {
    std::string id;
    std::string speaker;
    std::string text;
    std::vector<DialogChoice> choices;
};

std::map<std::string, DialogNode> buildDialogData();
