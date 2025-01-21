#include "CelestialObject.h"


vector<vector<bool>> CelestialObject::rotate_90_clockwise(const vector<vector<bool>>& shape) {
    int rows = shape.size();
    int cols = shape[0].size();
    vector<vector<bool>> rotated(cols, vector<bool>(rows, false));

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            rotated[j][rows - i - 1] = shape[i][j];
        }
    }
    return rotated;
}

// Constructor to initialize CelestialObject with essential properties and generate/link rotations
CelestialObject::CelestialObject(const vector<vector<bool>> &shape, ObjectType type, int start_row, int time_of_appearance)
    : shape(shape), object_type(type), starting_row(start_row), time_of_appearance(time_of_appearance) {

    vector<vector<bool>> rotation_90 = rotate_90_clockwise(shape);
    vector<vector<bool>> rotation_180 = rotate_90_clockwise(rotation_90);
    vector<vector<bool>> rotation_270 = rotate_90_clockwise(rotation_180);


    if (shape == rotation_90 && shape == rotation_180) {

        CelestialObject* rotation1 = this;
        rotation1->right_rotation = rotation1;
        rotation1->left_rotation = rotation1;
    }
    else if (shape == rotation_180) {

        CelestialObject* rotation1 = this;
        CelestialObject* rotation2 = new CelestialObject(rotation1);
        rotation2->shape = rotation_90;
        rotation1->right_rotation = rotation2;
        rotation2->left_rotation = rotation1;
        rotation2->right_rotation = rotation1;
        rotation1->left_rotation = rotation2;
    } else{

        CelestialObject* rotation1 = this;
        CelestialObject* rotation2 = new CelestialObject(rotation1);
        rotation2->shape = rotation_90;
        CelestialObject* rotation3 = new CelestialObject(rotation2);
        rotation3->shape = rotation_180;
        CelestialObject* rotation4 = new CelestialObject(rotation3);
        rotation4->shape = rotation_270;

        rotation1->right_rotation = rotation2;
        rotation2->left_rotation = rotation1;

        rotation2->right_rotation = rotation3;
        rotation3->left_rotation = rotation2;

        rotation3->right_rotation = rotation4;
        rotation4->left_rotation = rotation3;

        rotation4->right_rotation = rotation1;
        rotation1->left_rotation = rotation4;

    }
}

// Copy constructor for CelestialObject
CelestialObject::CelestialObject(const CelestialObject *other)
    : shape(other->shape), object_type(other->object_type), starting_row(other->starting_row), time_of_appearance(other->time_of_appearance) {

    next_celestial_object = nullptr;
    right_rotation = nullptr;
    left_rotation = nullptr;

}

void CelestialObject::delete_rotations(CelestialObject *target) {
    if (target == nullptr) return;

    CelestialObject *current = target->right_rotation;
    while (current != target) {
        CelestialObject *prev = current;
        current = current->right_rotation;
        delete prev;

    }
}