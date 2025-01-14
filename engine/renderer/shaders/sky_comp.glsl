#version 450
layout (local_size_x = 16, local_size_y = 16) in;
layout(rgba8,set = 0, binding = 0) uniform image2D image;


layout( push_constant ) uniform constants {
 vec4 data1;
 vec4 data2;
 vec4 data3;
 vec4 data4;
} PushConstants;

float Noise2d( in vec2 x ) {
    float xhash = cos( x.x * 37.0 );
    float yhash = cos( x.y * 57.0 );
    return fract( 415.92653 * ( xhash + yhash ) );
}

float NoisyStarField( in vec2 vSamplePos, float fThreshhold ) {
    float StarVal = Noise2d( vSamplePos );
    if ( StarVal >= fThreshhold )
        StarVal = pow( (StarVal - fThreshhold)/(1.0 - fThreshhold), 6.0 );
    else
        StarVal = 0.0;
    return StarVal;
}

float StableStarField( in vec2 vSamplePos, float fThreshhold ) {
    float fractX = fract( vSamplePos.x );
    float fractY = fract( vSamplePos.y );
    vec2 floorSample = floor( vSamplePos );    
    float v1 = NoisyStarField( floorSample, fThreshhold );
    float v2 = NoisyStarField( floorSample + vec2( 0.0, 1.0 ), fThreshhold );
    float v3 = NoisyStarField( floorSample + vec2( 1.0, 0.0 ), fThreshhold );
    float v4 = NoisyStarField( floorSample + vec2( 1.0, 1.0 ), fThreshhold );

    float StarVal =   v1 * ( 1.0 - fractX ) * ( 1.0 - fractY )
        			+ v2 * ( 1.0 - fractX ) * fractY
        			+ v3 * fractX * ( 1.0 - fractY )
        			+ v4 * fractX * fractY;
	return StarVal;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord ) {
    vec2 iResolution = imageSize(image);
    vec3 vColor = PushConstants.data1.xyz * fragCoord.y / iResolution.y;

    float StarFieldThreshhold = PushConstants.data1.w;

    float xRate = 0.2;
    float yRate = -0.06;
    vec2 vSamplePos = fragCoord.xy + vec2( xRate * float( 1 ), yRate * float( 1 ) );
	float StarVal = StableStarField( vSamplePos, StarFieldThreshhold );
    vColor += vec3( StarVal );
	
	fragColor = vec4(vColor, 1.0);
}

void main() {
	vec4 value = vec4(0.0, 0.0, 0.0, 1.0);
    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);
	ivec2 size = imageSize(image);
    if(texelCoord.x < size.x && texelCoord.y < size.y)
    {
        vec4 color;
        mainImage(color,texelCoord);
    
        imageStore(image, texelCoord, color);
    }   
}