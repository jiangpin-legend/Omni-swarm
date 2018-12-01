#include <iostream>
#include <aruco/aruco.h>
#include <opencv2/highgui.hpp>
#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc.hpp>
#include <swarm_detection/drone_pose_estimator.h>

typedef std::vector<aruco::Marker> marker_array;
class ARMarkerDetectorNode {
    ros::NodeHandle & nh;
    aruco::MarkerDetector MDetector;
    aruco::CameraParameters camera_left;
    aruco::CameraParameters camera_right;

    ros::Subscriber left_image_sub;
    ros::Subscriber right_image_sub;
public:
    ARMarkerDetectorNode(ros::NodeHandle & _nh):
        nh(_nh)
    {
        MDetector.setDictionary("ARUCO_MIP_36h12");
        // local_odometry_sub = nh.subscribe(vins_topic, 1, &SwarmDroneProxy::on_local_odometry_recv, this);
        left_image_sub = nh.subscribe("left_camera", 1, &ARMarkerDetectorNode::image_cb_left);
        right_image_sub = nh.subscribe("right_camera", 1, &ARMarkerDetectorNode::image_cb_right);
    }

    void read_camera_params(std::string left_camera, std::string right_camera) {
        
    }

    void run_swarm_pose_estimatior(marker_array left_cam_marker, marker_array right_cam_marker) {
        //Divide marker array to drones

        //Send to drone pose estimator
    }

    void detect_cv_image(const cv::Mat & img, int camera_id) {
        marker_array ma = MDetector.detect(img);
        for(auto m: ma){
            std::cout<<m<<std::endl;    
            m.draw(img);
        }
        if (camera_id == 0)
            cv::imshow("left", img);
        if (camera_id == 1)
            cv::imshow("right", img);
        cv::waitKey(10);


        //Run swarm pose estimator
    }

    void image_cb_left(const sensor_msgs::ImageConstPtr& msg) {
        image_cb(msg, 0);
    }

    void image_cb_right(const sensor_msgs::ImageConstPtr& msg) {
        image_cb(msg, 1);
    }

    void image_cb(const sensor_msgs::ImageConstPtr& msg, int camera_id)
    {
        cv_bridge::CvImageConstPtr cv_ptr;
        try
        {
            cv_ptr = cv_bridge::toCvShare(msg, enc::BGR8);

            this->detect_cv_image(cv_ptr->image, camera_id);
        }
        catch (cv_bridge::Exception& e)
        {
            ROS_ERROR("cv_bridge exception: %s", e.what());
            return;
        }
    }

};

int main(int argc,char **argv)
{
    ros::init(argc, argv, "swarm_detection");
    ros::NodeHandle nh("swarm_detection");

    ARMarkerDetectorNode ar_node(nh);
    
    ros::spin();

}