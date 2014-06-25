//
//  material.h
//  CG_Project
//
//  Created by Jan-Willem Gmelig Meyling on 25/06/14.
//  Copyright (c) 2014 TU Delft. All rights reserved.
//

#ifndef CG_Project_material_h
#define CG_Project_material_h


class Material
{
    
public:
    
    Material() { cleanup(); };
    
    Material(const Material & m)
    {
        *this=m;
    };
    
    Material & operator=(const Material & m)
    {
        Kd_=m.Kd_;
        Kd_is_set_=m.Kd_is_set_; // diffuse
        Ka_=m.Ka_;
        Ka_is_set_=m.Ka_is_set_; // ambient
        Ks_=m.Ks_;
        Ks_is_set_=m.Ks_is_set_; // specular
        
        Ns_=m.Ns_;
        Ns_is_set_=m.Ns_is_set_; // specular shinyness
        Ni_=m.Ni_;
        Ni_is_set_=m.Ni_is_set_; // specular
        
        Tr_=m.Tr_;
        Tr_is_set_=m.Tr_is_set_; // transperency
        illum_ = m.illum_;
        name_=m.name_;
        return (*this);
    };
    
    void cleanup()
    {
        Kd_is_set_ = false;
        Ka_is_set_ = false;
        Ks_is_set_ = false;
        Ns_is_set_ = false;
        Ni_is_set_ = false;
        Tr_is_set_ = false;
        illum_is_set_=false;
        name_="empty";
    }
    
    bool is_valid(void) const
    { return Kd_is_set_ || Ka_is_set_ || Ks_is_set_ || Tr_is_set_; }
    
    bool has_Kd(void) { return Kd_is_set_; }
    bool has_Ka(void) { return Ka_is_set_; }
    bool has_Ks(void) { return Ks_is_set_; }
    bool has_Ns(void) { return Ns_is_set_; }
    bool has_Ni(void) { return Ni_is_set_; }
    bool has_illum(void) { return illum_is_set_; }
    bool has_Tr(void) { return Tr_is_set_; }
    
    void set_Kd( float r, float g, float b )
    { Kd_=Vec3Df(r,g,b); Kd_is_set_=true; }
    
    void set_Ka( float r, float g, float b )
    { Ka_=Vec3Df(r,g,b); Ka_is_set_=true; }
    
    void set_Ks( float r, float g, float b )
    { Ks_=Vec3Df(r,g,b); Ks_is_set_=true; }
    
    void set_Ns( float r)
    { Ns_=r;    Ns_is_set_=true; }
    
    void set_Ni( float r)
    { Ni_=r;    Ni_is_set_=true; }
    
    void set_illum( int r)
    { illum_=r;    illum_is_set_=true; }
    
    void set_Tr( float t )
    { Tr_=t;            Tr_is_set_=true; }
    
    void set_textureName(const std::string & s)//name of the texture image file
    {
        textureName_=s;
    }
    
    void set_name(const std::string & s )
    {
        name_=s;
    }
    
    const Vec3Df& Kd( void ) const { return Kd_; } //diffuse
    const Vec3Df& Ka( void ) const { return Ka_; } //ambiant
    const Vec3Df& Ks( void ) const { return Ks_; } //specular
    float  Ni( void ) const { return Ni_; }
    float  Ns( void ) const { return Ns_; } //shininess
    int       illum(void)const { return illum_;}
    float  Tr( void ) const { return Tr_; }//can be hijacked, e.g., for transparency
    const std::string & textureName()//name of the texture image file
    {
        return textureName_;
    }
    
    const std::string & name()//name of the material
    {
        return name_;
    }
    
    
    static Material DiffuseWhite;
    static Material SoftPink;
    static Material BlueSemiTransparent;
    static Material Mirror;
    
private:
    
    Vec3Df Kd_;         bool Kd_is_set_; // diffuse
    Vec3Df Ka_;         bool Ka_is_set_; // ambient
    Vec3Df Ks_;         bool Ks_is_set_; // specular
    float Ns_;                     bool Ns_is_set_;
    float Ni_;                     bool Ni_is_set_;
    int illum_;                     bool illum_is_set_;//illumination model
    float Tr_;         bool Tr_is_set_; // transperency
    std::string        name_;
    std::string        textureName_;
};

class MaterialBuilder {
public:
    MaterialBuilder& set_Kd(const Vec3Df &Kd) {
        Kd_ = Kd; Kd_is_set_ = true;
        return *this;
    };
    
    MaterialBuilder& set_Ka(const Vec3Df &Ka) {
        Ka_ = Ka; Ka_is_set_=true;
        return *this;
    }
    
    MaterialBuilder& set_Ks(const Vec3Df &Ks) {
        Ks_ = Ks; Ks_is_set_=true;
        return *this;
    }
    
    MaterialBuilder& set_Ns( float r) {
        Ns_ = r;    Ns_is_set_=true;
        return *this;
    }
    
    MaterialBuilder& set_Ni( float r) {
        Ni_=r;    Ni_is_set_=true;
        return *this;
    }
    
    MaterialBuilder& set_illum( int r) {
        illum_=r;    illum_is_set_=true;
        return *this;
    }
    
    MaterialBuilder& set_Tr( float t ) {
        Tr_=t;            Tr_is_set_=true;
        return *this;
    }
    
    Material build() {
        Material material = Material();
        if(Kd_is_set_) material.set_Kd(Kd_[0], Kd_[1], Kd_[2]);
        if(Ka_is_set_) material.set_Ka(Ka_[0], Ka_[1], Ka_[2]);
        if(Ks_is_set_) material.set_Ks(Ks_[0], Ks_[1], Ks_[2]);
        if(Ns_is_set_) material.set_Ns(Ns_);
        if(Ni_is_set_) material.set_Ni(Ni_);
        if(illum_is_set_) material.set_illum(illum_);
        if(Tr_is_set_) material.set_Tr(Tr_);
        return material;
    }
    
private:
    Vec3Df Kd_;        bool Kd_is_set_; // diffuse
    Vec3Df Ka_;        bool Ka_is_set_; // ambient
    Vec3Df Ks_;        bool Ks_is_set_; // specular
    float Ns_;         bool Ns_is_set_; // shinyness
    float Ni_;         bool Ni_is_set_; // refraction index
    int illum_;        bool illum_is_set_;//illumination model
    float Tr_;         bool Tr_is_set_; // transperency
};



#endif
