varying vec3 N;
varying vec3 L;
varying vec3 E;

void main()
{
    vec4 a = vec4(0.0,0.5,0.8,1.0);
    vec4 b = vec4(0.0,0.3,0.6,1.0);
    vec4 c = vec4(0.0,0.2,0.5,1.0);
    vec4 d = vec4(0.0,0.3,0.6,1.0);
    vec4 e = vec4(0.0,0.0,0.0,1.0);
    
    //PARTE 1
    vec4 f = vec4(1.0,0.0,0.0,1.0);
    
    vec4 color;
    float intensity = dot(normalize(L),normalize(N));

    if (intensity > 0.95)
        color = a;
    else if (intensity > 0.5)
        color = b;
    else if (intensity > 0.25)
        color = c;
    else if (intensity > 0.15)
        color = d;
    else
        color = e;

	// adjust the color to produce an outline/silhouette
    //PARTE 1
    float angle = dot(normalize(E),normalize(N));
    if(angle < 0.4)
        color = f;
    
    gl_FragColor = color;
}
