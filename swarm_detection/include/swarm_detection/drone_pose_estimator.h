#pragma once
#include <eigen3/Eigen/Eigen>
#include <vector>
#include <map>
#include <assert.h>
#include "ceres/rotation.h"
#include "ceres/ceres.h"

using namespace Eigen;
using namespace ceres;

struct Camera {
    Eigen::Affine3d trans;
    double fx, fy, cx, cy;
    double k1, k2, p1, p2;
    int size_w;
    int size_h;

    template <typename T> inline
    void project_to_camera(const T point3d[3], T point[2])  const {
        point[0] = point3d[0] / point3d[2];
        point[1] = point3d[1] / point3d[2];

        T d_u[2];
        distortion(point, d_u);
        point[0] = point[0] + d_u[0];
        point[1] = point[1] + d_u[1];

        point[0] = fx * point[0] + cx;
        point[1] = fy * point[1] + cy;
    }

    template <typename T> inline
    void distortion(const T point[2], T d_u[2]) const {
        T mx2_u = point[0] * point[0];
        T my2_u = point[1] * point[1];
        T mxy_u = point[0] * point[1];
        T rho2_u = mx2_u + my2_u;
        T rad_dist_u = k1 * rho2_u + k2 * rho2_u * rho2_u;

        d_u[0] = point[0] * rad_dist_u + 2.0 * p1 * mxy_u + p2 * (rho2_u + 2.0 * mx2_u);
        d_u[1] = point[1] * rad_dist_u + 2.0 * p2 * mxy_u + p1 * (rho2_u + 2.0 * my2_u);
    }

    Vector3d pos_on_drone;
    Quaterniond att_on_drone;

    Quaterniond att() const {
        return att_on_drone;
    }

    Vector3d pos() const {
        return pos_on_drone;
    }
};

struct DroneMarker {
    //Trans is relative to own drone
    Eigen::Affine3d trans;
    int id;
    int drone_id;
    double size;

    //Corner position on the drone
    Eigen::Vector3d rel_corner_pos(int corner_no) const {
    switch (corner_no) {
        case 0:
            return Eigen::Vector3d(0, size/2, size/2);
            break;
        case 1:
            return Eigen::Vector3d(0, -size/2, size/2);
            break;
        case 2:
            return Eigen::Vector3d(0, -size/2, -size/2);
            break;
        case 3:
            return Eigen::Vector3d(0, size/2, -size/2);
            break;
    }
    return Eigen::Vector3d(0, 0, 0);
}

    DroneMarker(int _id, int _drone_id, Eigen::Affine3d _trans, double _size):
        size(_size), id(_id), trans(_trans), drone_id(_drone_id)
    {}

};

typedef std::map<int, DroneMarker*> marker_dict;

class MarkerCornerObervsed {
public:
    int id;
    int corner_no;
    Eigen::Vector2d observed_point;
    DroneMarker * marker = nullptr;
    Eigen::Vector3d rel_corner_pos() const {
        assert(marker!=nullptr && "Must like corner to a marker before use relative corner position");
        return marker->rel_corner_pos(this->id);
    }

    MarkerCornerObervsed(int _id, int _corner_no, DroneMarker * _marker):
        id(_id), corner_no(_corner_no), marker(_marker)
    {

    }
};

typedef std::vector<Camera*> camera_array;
typedef std::vector<MarkerCornerObervsed> corner_array;


template <typename T>
inline void InverseTrans(const T pose[7], T pose_output[7]) {
    pose_output[0] = - pose[0];
    pose_output[1] = pose[1];
    pose_output[2] = pose[2];
    pose_output[3] = pose[3];
    pose_output[4] = - pose[4];
    pose_output[5] = - pose[5];
    pose_output[6] = - pose[6];
}

template <typename T>
inline void AppleTrans2Point(const T pose[7],const T point[3], T p[3]) {
    QuaternionRotatePoint(pose, point, p);
    p[0] += pose[4];
    p[1] += pose[5];
    p[2] += pose[6];
}

template <typename T>
inline void MultiplyTrans(const T posea[7],const T poseb[7], T ret[7]) {
    QuaternionProduct(posea, poseb, ret);
    T pointb[3] ;
    T respoint[3];
    
    pointb[0] = poseb[4];
    pointb[1] = poseb[5];
    pointb[2] = poseb[6];

    QuaternionRotatePoint(posea, pointb, respoint);
    ret[4] = respoint[0] + posea[0];
    ret[5] = respoint[1] + posea[1];
    ret[6] = respoint[2] + posea[2];
}

template <typename T>
inline void TransFromVecQuat(const Vector3d vec, const Quaterniond quat,T trans[7]) {
    trans[0] = T(quat.w());
    trans[1] = T(quat.x());
    trans[2] = T(quat.y());
    trans[3] = T(quat.z());
    trans[4] = T(vec.x());
    trans[5] = T(vec.y());
    trans[6] = T(vec.z());
}


struct DronePoseReprojectionError {
    std::vector<corner_array> point_by_cam;
    camera_array cam_array;

    DronePoseReprojectionError(std::vector<corner_array> _point_by_cam, camera_array _ca) : 
        point_by_cam(_point_by_cam), cam_array(_ca)
    {

    }

    
    template <typename T>
    bool operator()(const T* const pose,
                    T* residuals) const {
        //Pose 0,1,2,3 quat
        //Pose 4,5,6 position
        int res_count = 0;
        for (int i = 0; i < point_by_cam.size(); i ++) {
            const Camera * cam_def = cam_array[i];
            for (MarkerCornerObervsed mco : point_by_cam[i]) {
                //Calculate a residual
                // residuals[res_count] = 
                //proj(Tcam^-1 * Tik * Pmpni) -Zn
                //Tik is pose of target drone, i.e pose
                T cam_pose[7];
                T _tmp[7];
                T _tmp2[7];

                TransFromVecQuat(cam_def->pos(), cam_def->att(), cam_pose);

                T Pmpni[3];
                Vector3d pmpni = mco.rel_corner_pos();
                Pmpni[0] = T(pmpni.x());
                Pmpni[1] = T(pmpni.y());
                Pmpni[2] = T(pmpni.z());

                T point_to_proj[3];
                
                InverseTrans(cam_pose, _tmp);
                MultiplyTrans(_tmp, pose, _tmp2);

                AppleTrans2Point(_tmp2, Pmpni, point_to_proj);

                //Point should on camera
                T predict_point[2];
                cam_def->project_to_camera(point_to_proj, predict_point);

                residuals[res_count] = predict_point[0] - mco.observed_point.x();
                res_count ++;
                residuals[res_count] = predict_point[1] - mco.observed_point.y();
                res_count ++;
            }
        }
        return true;
    }

    // Factory to hide the construction of the CostFunction object from
    // the client code.
    static ceres::CostFunction* Create(std::vector<corner_array> _point_by_cam, camera_array _ca) {
        return (new ceres::AutoDiffCostFunction<
                DronePoseReprojectionError, ceres::DYNAMIC, 7>(
                    new DronePoseReprojectionError( _point_by_cam, _ca)));
    }

};

class DronePoseEstimator {
    camera_array cam_defs;
    marker_dict md;
public:
    DronePoseEstimator(camera_array _ca);

    //point_by_cam is point corre to camera. length must equal to camera
    //marker is the id of these markers
    //All this marker must belong to one drone
    Eigen::Affine3d estimation_drone_pose(std::vector<corner_array> point_by_cam);
};