
module FrameHelper::Base::Samples::Frame
    class Frame
        def self.to_native(obj,context)
            if obj.is_a?(Typelib::Type) && obj.class.name == '/base/samples/frame/Frame'
                s = FrameHelper::Base::Samples::Frame::FrameStruct.new
                s[:obj_ptr]  = obj.to_memory_ptr.zone_address
                s[:bowner] = false
                s[:size] = 0 #obj.marshalling_size
                s[:type_id] = nil
                s[:version] = 1
                s
            else
                rbind_to_native(obj,context)
            end
        end
    end
end

