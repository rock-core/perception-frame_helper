require 'frame_helper'
require 'vizkit'

include FrameHelper
include OpenCV

replay = Orocos::Log::Replay.open(ARGV)
port = replay.find_all_ports("/base/samples/frame/Frame").find do|port|
    port.number_of_samples > 0
end

Orocos.initialize
Orocos.load_typekit 'base'

display = Vizkit.default_loader.ImageView
display.show

port.on_data do |sample|
    mat = FrameHelper::FrameHelper.convert_to_cv_mat(sample)
    cv.blur(mat,mat,cv::Size.new(20,20))
    f = Types::Base::Samples::Frame::Frame.new
    FrameHelper::FrameHelper.copy_mat_to_frame(mat,f)
    display.display f
end

Vizkit.control replay
Vizkit.exec
