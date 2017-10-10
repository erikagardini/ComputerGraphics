#include "raytracer.h"
#include "material.h"
#include "vectors.h"
#include "argparser.h"
#include "raytree.h"
#include "utils.h"
#include "mesh.h"
#include "face.h"
#include "sphere.h"

// casts a single ray through the scene geometry and finds the closest hit
bool
RayTracer::CastRay (Ray & ray, Hit & h, bool use_sphere_patches) const
{
  bool answer = false;
  Hit nearest;
  nearest = Hit();

  // intersect each of the quads
  for (int i = 0; i < mesh->numQuadFaces (); i++)
  {
    Face *f = mesh->getFace (i);
    if (f->intersect (ray, h, args->intersect_backfacing))
    {
        if( h.getT() < nearest.getT() )
        {
            answer = true;
            nearest = h;
        }
    }
  }

  // intersect each of the spheres (either the patches, or the original spheres)
  if (use_sphere_patches)
  {
    for (int i = mesh->numQuadFaces (); i < mesh->numFaces (); i++)
    {
      Face *f = mesh->getFace (i);
      if (f->intersect (ray, h, args->intersect_backfacing))
      {
        if( h.getT() < nearest.getT() )
        {
            answer = true;
            nearest = h;
        }
      }
    }
  }
  else
  {
    const vector < Sphere * >&spheres = mesh->getSpheres ();
    for (unsigned int i = 0; i < spheres.size (); i++)
    {
      if (spheres[i]->intersect (ray, h))
      {
        if( h.getT() < nearest.getT() )
        {
            answer = true;
            nearest = h;
        }
      }
    }
  }

  h = nearest;
  return answer;
}

Vec3f
RayTracer::TraceRay (Ray & ray, Hit & hit, int bounce_count) const
{
  //hit è il punto di intersezione;
  hit = Hit ();
  bool intersect = CastRay (ray, hit, false);

  if( bounce_count == args->num_bounces )
      RayTree::SetMainSegment (ray, 0, hit.getT ());
  else
    RayTree::AddReflectedSegment(ray, 0, hit.getT());

//carichiamo il background perchè questo c'è a prescindere
  Vec3f answer = args->background_color;

  if (intersect == true)
  {
    Material *m = hit.getMaterial ();
    assert (m != NULL);
    Vec3f normal = hit.getNormal ();
    Vec3f point = ray.pointAtParameter (hit.getT ());

    answer = args->ambient_light * m->getDiffuseColor ();



    int num_lights = mesh->getLights().size ();
//      Vec3f pointOnLight;
//      Vec3f dirToLight;
//      Vec3f lightColor;
      int j;
    for (int i = 0; i < num_lights; i++)
    {
        Face *f = mesh->getLights ()[i];
        int num_shadows = args->num_shadow_samples;
        if(num_shadows == 0){
            Vec3f pointOnLight = f->computeCentroid ();
            Vec3f dirToLight = pointOnLight - point;
            dirToLight.Normalize ();
            if (normal.Dot3 (dirToLight) > 0){
                Vec3f lightColor = 0.2 * f->getMaterial()->getEmittedColor() * f->getArea ();
                answer += m->Shade (ray, hit, dirToLight, lightColor, args);
            }
        }else if(num_shadows == 1 || f->getArea() == 0){
            Vec3f pointOnLight = f->computeCentroid ();
            Vec3f dirToLight = pointOnLight - point;
            dirToLight.Normalize ();
            Ray *shadowRay = new Ray(point, dirToLight);
            Hit *newHit = new Hit();
            bool colpito = CastRay(*shadowRay, *newHit, 0);
            if(colpito){
                Vec3f n_point = shadowRay->pointAtParameter(newHit->getT());
                Vec3f dista;
                dista.Sub(dista, n_point, pointOnLight);
                if(dista.Length() < 0.01){
                    if (normal.Dot3 (dirToLight) > 0){
                        Vec3f lightColor = 0.2 * f->getMaterial()->getEmittedColor() * f->getArea ();
                        answer += m->Shade (ray, hit, dirToLight, lightColor, args);
                    }
                }
            }
        }else{
            Vec3f answer1;
            for(j = 0; j < num_shadows; j++){
                Vec3f pointOnLight = f->RandomPoint();
                Vec3f dirToLight = pointOnLight - point;
                dirToLight.Normalize ();
                Ray *shadowRay = new Ray(point, dirToLight);
                Hit *newHit = new Hit();
                bool colpito = CastRay(*shadowRay, *newHit, 0);
                if(colpito){
                    Vec3f n_point = shadowRay->pointAtParameter(newHit->getT());
                    Vec3f dista;
                    dista.Sub(dista, n_point, pointOnLight);
                    if(dista.Length() < 0.01){
                        if (normal.Dot3 (dirToLight) > 0){
                            Vec3f lightColor = 0.2 * f->getMaterial()->getEmittedColor() * f->getArea ();
                            answer1 += m->Shade (ray, hit, dirToLight, lightColor, args);
                        }
                    }
                }
            }
            answer1 /= num_shadows;
            answer += answer1;
        }

        }

      Vec3f reflectiveColor = m->getReflectiveColor ();
      if(reflectiveColor.Length() != 0 && bounce_count > 0){
          Vec3f rayVector = ray.getOrigin();
          Vec3f reflection = (2 * normal.Dot3(rayVector) * normal) - rayVector;
          reflection.Normalize();
          Ray ray = Ray(point, reflection);
          answer += TraceRay(ray, hit, bounce_count - 1);
          answer = answer * reflectiveColor;
      }

  }
  return answer;
}

