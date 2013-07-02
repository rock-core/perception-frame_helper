require 'rbind'

rbind = Rbind::Rbind.new("FrameHelper")
rbind.generator_ruby.compact_namespace = true
rbind.pkg_config << "base-types"
rbind.gems << "ropencv"
rbind.includes = [File.absolute_path(File.join(File.dirname(__FILE__),"..","..","src","FrameHelperTypes.h")),
                  File.absolute_path(File.join(File.dirname(__FILE__),"..","..","src","FrameHelper.h"))]

rbind.parse File.join(File.dirname(__FILE__),"types.txt")
rbind.parse_extern

rbind.use_namespace rbind.cv
rbind.parse_headers
rbind.libs << "frame_helper"
rbind.generate(File.join(File.dirname(__FILE__),"src"),File.join(File.dirname(__FILE__),"lib","ruby","frame_helper"))
