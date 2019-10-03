#ifndef VECTOR_H
#define VECTOR_H

// TODO: rename back to vector
class RFVector {
// TODO: make the member variables private
public:
    float i = 0;
    float j = 0;
    float k = 0;
    float s = 0;
    float t = 0;

//public:
    RFVector();
    //template <class vec>Vector(const vec &in);
    //Vector(const Vector &in);
    RFVector(float x, float y, float z);
    RFVector(float x, float y, float z, float s, float t);

    float Mag();
    RFVector Cross( const RFVector &v ) const;

    // TODO: consider moving from here
    void Yaw(float rad); //only works with unit vector
    void Roll(float rad);
    void Pitch(float rad);
};

#endif // VECTOR_H
