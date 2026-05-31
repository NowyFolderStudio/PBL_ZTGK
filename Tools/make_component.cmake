# Checking if the component name is provided
if(NOT DEFINED COMP_NAME)
    message(FATAL_ERROR "Component name not provided! Usage: cmake -DCOMP_NAME=ComponentName -P make_component.cmake")
endif()

# Defining the output directory for the new component
set(OUTPUT_DIR "${CMAKE_CURRENT_LIST_DIR}/../Game/include/Components")
# Checking path validity
set(FILE_PATH "${OUTPUT_DIR}/${COMP_NAME}.hpp")

# Checking if the file already exists
if(EXISTS "${FILE_PATH}")
    message(FATAL_ERROR "File ${FILE_PATH} already exists!")
endif()

# Template content for the new component
set(TEMPLATE_CONTENT "#pragma once

#include <NFSEngine.h>

class ${COMP_NAME} : public NFSEngine::Component {
public:
    ${COMP_NAME}(NFSEngine::GameObject* owner)
        : NFSEngine::Component(owner) { }

    std::string GetName() const override { return \"${COMP_NAME}\"; }

protected:
    void OnAwake() override { }

    void OnStart() override { }

    void OnFixedUpdate(NFSEngine::DeltaTime deltaTime) override { }

    void OnUpdate(NFSEngine::DeltaTime deltaTime) override { }

    void OnEnable() override { }

    void OnDisable() override { }
};
")

# Writing to the file
file(WRITE "${FILE_PATH}" "${TEMPLATE_CONTENT}")
message(STATUS "SUCCESS: New component generated: ${FILE_PATH}")