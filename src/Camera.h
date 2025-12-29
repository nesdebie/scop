#ifndef CAMERA_H
#define CAMERA_H

#include "my_glm.h"

/**
 * Controls camera position and orientation in 3D space.
 * 
 * Manages camera view transformations using spherical coordinates
 * (yaw, pitch, distance) and model offset for orbiting around objects.
 */
class Camera {
    public:
        Camera();
        ~Camera();

        void init(float objectRadius);
        void reset(float objectRadius);

        my_glm::mat4 computeViewMatrix() const;
        my_glm::vec3 getCameraPosition() const;

        void updateYaw(float delta);
        void updatePitch(float delta);
        void updateDistance(float delta);
        void updateOffset(const my_glm::vec3& delta);

        float getYaw() const { return cameraYaw; }
        float getPitch() const { return cameraPitch; }
        float getDistance() const { return cameraDistance; }
        my_glm::vec3 getOffset() const { return modelOffset; }
        
        void setYaw(float yaw) { cameraYaw = yaw; }
        void setPitch(float pitch) { cameraPitch = pitch; }
        void setDistance(float distance) { cameraDistance = distance; }
        void setOffset(const my_glm::vec3& offset) { modelOffset = offset; }

    private:
        float cameraYaw;
        float cameraPitch;
        float cameraDistance;
        my_glm::vec3 modelOffset;
};

#endif
