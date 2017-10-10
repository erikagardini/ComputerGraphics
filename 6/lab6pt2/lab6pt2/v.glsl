uniform float time;
attribute  float vx, vy, vz; //add velocity in z-direction
const float a = -0.000001;

void main()
{
    
    vec4 t = gl_Vertex;
    t.y = gl_Vertex.y + vy*time + 0.5*a*time*time;
    t.x = gl_Vertex.x + vx*time;
	
    //utilize t.z and gl_PointSize
    
    //PARTE 1
    gl_PointSize = min(t.x * t.y, 0.1);
    //FINE PARTE 1
    
    //PARTE 2
    t.z = gl_Vertex.z + vz*time;
    //FINE PARTE 2
    
    gl_Position = gl_ModelViewProjectionMatrix*t;
    gl_FrontColor =  gl_Color;
}
