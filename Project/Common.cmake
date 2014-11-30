macro(_add_package_VulkanSDK)
  find_package(VulkanSDK)  
  if(VULKANSDK_FOUND)
      Message(STATUS "--> using package VulkanSDK")
      add_definitions(-DUSEVULKANSDK)
      include_directories(${VULKANSDK_INCLUDE_DIR})
    LIST(APPEND LIBRARIES_OPTIMIZED ${VULKAN_LIB} )
      LIST(APPEND LIBRARIES_DEBUG ${VULKAN_LIB} )
      LIST(APPEND PACKAGE_SOURCE_FILES ${VULKANSDK_HEADERS} )
      source_group(OPTIX FILES  ${VULKANSDK_HEADERS} )
 else()
     Message(STATUS "--> NOT using package VulkanSDK")
 endif()
endmacro()