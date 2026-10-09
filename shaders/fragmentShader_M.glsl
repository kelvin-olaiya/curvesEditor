#version 330 core
in vec4 ourColor;
out vec4 FragColor;  
 
uniform int sceltaFS;
uniform float time;
uniform vec2 res;
uniform vec2 mouse;
#define PI 3.1415


float circle(vec2 c, float r, vec2 p) {
	
	if (length(c-p) < r) { return 1.0; }
	return 0.0;
}

void main()
{
   if (sceltaFS==0)
    {
        FragColor = vec4(ourColor);
    }
    if (sceltaFS==1)
    {
    vec2 coord = gl_FragCoord.xy /res;

    FragColor=vec4(1-sin(coord.x),0.0,1-cos(coord.y ),1 );
    } 
    

     if (sceltaFS==2){

	vec2 p = (gl_FragCoord.xy / res.xy);
	vec3 color = vec3(0.8);
	
	p.y += sin(p.x * 5.0 ) * 0.1;
	
	if (distance(vec2(0.0), vec2(0.0, p.y)) < 1 && distance(vec2(0.0), vec2(0.0, p.y)) > 0.1)
		color = vec3(1.0, 0.0, 0.0) + clamp(0.0, 0.0,(sin(p.x * 5.0  + PI) * 0.1) * 5.0);	
	if (distance(vec2(0.0), vec2(0.0, p.y)) < (1.0 / 6.0) * 5.0 && distance(vec2(0.0), vec2(0.0, p.y)) > 0.1)
		color = vec3(1.0, 0.5, 0.0) + clamp(0.0, 0.0,(sin(p.x * 5.0  + PI) * 0.1) * 5.0);
	if (distance(vec2(0.0), vec2(0.0, p.y)) < (1.0 / 6.0) * 4.0 && distance(vec2(0.0), vec2(0.0, p.y)) > 0.1)
		color = vec3(1.0, 1.0, 0.0) + clamp(0.0, 0.0,(sin(p.x * 5.0  + PI) * 0.1) * 5.0);
	if (distance(vec2(0.0), vec2(0.0, p.y)) < (1.0 / 6.0) * 3.0 && distance(vec2(0.0), vec2(0.0, p.y)) > 0.1)
		color = vec3(0.0, 0.8, 0.0) + clamp(0.0, 0.0,(sin(p.x * 5.0 + PI) * 0.1) * 5.0);
	if (distance(vec2(0.0), vec2(0.0, p.y)) < (1.0 / 6.0) * 2.0 && distance(vec2(0.0), vec2(0.0, p.y)) > 0.1)
		color = vec3(0.0, 0.0, 1.0) + clamp(0.0, 0.0,(sin(p.x * 5.0 + PI) * 0.1) * 5.0);
	if (distance(vec2(0.0), vec2(0.0, p.y)) < (1.0 / 6.0) * 1.0 && distance(vec2(0.0), vec2(0.0, p.y - 1.0)) < 1.0)
		color = vec3(0.8, 0.0, 1.0) + clamp(0.0, 0.0,(sin(p.x * 5.0 + PI) * 0.1) * 5.0);
	

	 FragColor = vec4(color, 1.0 );

}

  if (sceltaFS==3){


vec2 position = ( gl_FragCoord.xy / res.xy ) + mouse / 4.0;

	float color = 0.0;
	color += sin( position.x * cos( time / 15.0 ) * 80.0 ) + sin( position.y * sin( time / 15.0 ) * 10.0 );
	color += sin( position.y * sin( time / 10.0 ) * 40.0 ) + cos( position.x * sin( time / 25.0 ) * 40.0 );
	color += sin( position.x * sin( time / 5.0 ) * 10.0 ) + sin( position.y * sin( time / 35.0 ) * 80.0 );
	color *= sin( time / 10.0 ) * 0.5;

	 FragColor = vec4( vec3( color, color * 0.5, sin( color + time / 3.0 ) * 0.75 ), 1.0 );
}

if (sceltaFS==4){

	vec2 pos = ( gl_FragCoord.xy - res  / 2.0) / res.y;
	//if (intensity > 0.99)
	
	float angle = acos(dot(vec2(0.0, 1.0), normalize(pos)))* sign(pos.x) / PI * 1.0 + time / 1.0;
	float koef = (sin(time + length(pos)) * 0.5 + 0.5) * 0.5 + 0.5;
	float intensity = 1.0 - mod(length(pos) * koef * 10.0 + angle, 1.0);
	
	vec3 col = mix(vec3(1.0,0.0,0.0), vec3(0.0,0.0,1.0), length(pos));
	//	col = vec3(0.0,1.0,9.0);
	FragColor = vec4( col * intensity, 1.0 );
}

if (sceltaFS==5){


	vec2 position = ( gl_FragCoord.xy / res.xy )+mouse/800;

	float b = circle(vec2(.5,.5), .2, position);
	
	

	FragColor = vec4( position.x, position.y, b, 1.0 );

}


 
}