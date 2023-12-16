import os

from SetupPython import PythonConfiguration as PythonRequirements

PythonRequirements.Validate()

from SetupVulkan import VulkanConfiguration as VulkanRequirements

os.chdir('./../')

VulkanRequirements.Validate()