# Graphics

Windows rendering pipeline brief
* Application - An application needs to pass data to Windows
* Windows - Direct3D Runtime, Context Queue, DXD Kernel Scheduler - Windows needs to manage the process and interface with the drivers
* Driver - Direct3D User Mode Driver (UMD), Kernel Mode Driver
* Hardware or GPU - Once Windows and driver preparation is complete, a frame can be passed off to the GPU for final rendering and display

Windows rendering pipeline
* Simulator updates game world (object positions, physics, etc.)
* Renderer creates Draw calls via Direct3D API to generate a frame
* Direct3D runtime parses commands and calls via User Mode Driver (UMD) via DDI interface
* User Mode Driver creates GPU command buffers and submits them to Windows
* Command buffers are placed in a context queue until the OS kernel scheduler is ready to accept them
* Command buffers are passed to the Kernel Mode Driver, which processes them and sends them to the GPU
* GPU hardware sends an interrupt to the Kernel Mode Driver when execution is complete
* Application sends Present calls when a frame has finished rendering and can be displayed
* Present calls follow the same logical path as Draw calls

UMD is just a dll - "nvd3dum.dll" (NVidia) or "atiumd*.dll" (AMD) -  implements a lower-level API (the DDI) that is called by D3D

The context queue is responsible for queuing the individual command buffers in order to smooth out the rendering process

Queuing command buffers at this stage increases frame rendering latency, but by providing a buffer of buffers it allows the rendering pipeline to absorb any variances in rendering time or simulation time to more smoothly render frames

Beyond the context queue we have Windows' GPU scheduler, which is what regulates the popping of command buffers off of the context queue to be fed to the kernel mode GPU driver (KMD). Beyond this point the rest of the pipeline is rather simple, with the KMD taking the command buffer and feeding it to the GPU, all the while the KMD and GPU work together to manage the operation of the GPU. When a frame is finally completed, the GPU generates an interrupt to inform the KMD and OS about the completion

At the end of this process we have a rendered frame sitting in the GPU's back-buffer, but the frame itself is not displayed automatically. At the end of a batch of command buffers -- effectively making the beginning and ends of frames -- is the Direct3D Present() call. Present is the command that is responsible for telling the GPU to flip the back buffer to the front and to present the rendered frame to the user. Only once the Present call executes does a frame get displayed. The Present call, though not a command buffer object, still follows the same rendering path as the command buffers, including queuing up in the Context Queue

GPU Memory
* GPUs get a massive increase in bandwidth, but they pay for it with a massive increase in latency (and, it turns out, a sizable hit in power draw too, but that's beyond the scope of this article). This is part of a general pattern -- GPUs are all about throughput over latency; don't wait for results that aren't there yet, do something else instead!
* If you want to saturate memory bandwidth, you better do it one full DRAM row at a time.

PCIe host interface
* The bandwidth is decent though -- up to about 8GB/s (theoretical) peak aggregate bandwidth across the 16-lane PCIe 2.0 connections that most GPUs use right now, so between half and a third of the aggregate CPU memory bandwidth; that's a usable ratio. 
* We have a command buffer prepared on the CPU. We have the PCIe host interface, so the CPU can actually tell us about this, and write its address to some register. We have the logic to turn that address into a load that will actually return data -- if it's from system memory it goes through PCIe, if we decide we'd rather have the command buffer in video memory, the KMD can set up a DMA transfer so neither the CPU nor the shader cores on the GPU need to actively worry about it. And then we can get the data from our copy in video memory through the memory subsystem. All paths accounted for, we're set and finally ready to look at some commands!

Сортировка объектов рендеринга важна и при наличии буфера глубины: для правильной работы прозрачности и для оптимизации

GPU
+ Картинка, отображаемая монитором, находится в памяти GPU
+ PCI имеет огромную задержку
+ Shader != CPU code (следует избегать if и т.д.)
+ Command buffer or "Pushbuffer" - part of our app memory with commands that GPU read and do

gpu supports: per-fragment atomic lists - может помочь при рендеринге фейд-ин и фейд-аут наслаивающихся/пересекающихся объектов

## OpenGL

IRIS GL (Integrated Raster Imaging System Graphics Library) is a proprietary graphics API created by Silicon Graphics (SGI) in the early 1980s for producing 2D and 3D computer graphics on their IRIX-based IRIS graphical workstations. Later SGI removed their proprietary code, reworked various system calls, and released IRIS GL as the industry standard OpenGL

"DC" - Device Context
HDC - Handle "DC"
OpenGL "RC" (Rendering Context)

On Windows and in OpenGL we have 2 different ways of clear screen
Windows with DC
```C
FillRect(MyDC, ...);
```

Opengl with RC
```C
Clear(...);
```

If we want do it in specific window on Windows then need create RC based on DC (in current thread)
```C
internal void
Win32InitOpenGL(HWND Window)
{
    HDC WindowDC = GetDC(Window);

    // TODO(casey): Hey Raymond Chen - what's the deal here?
    // Is cColorBits ACTUALLY supposed to exclude the alpha bits, like MSDN says, or not?
    PIXELFORMATDESCRIPTOR DesiredPixelFormat = {};
    DesiredPixelFormat.nSize = sizeof(DesiredPixelFormat);
    DesiredPixelFormat.nVersion = 1;
    DesiredPixelFormat.iPixelType = PFD_TYPE_RGBA;
    DesiredPixelFormat.dwFlags = PFD_SUPPORT_OPENGL|PFD_DRAW_TO_WINDOW|PFD_DOUBLEBUFFER;
    DesiredPixelFormat.cColorBits = 32;
    DesiredPixelFormat.cAlphaBits = 8;
    DesiredPixelFormat.iLayerType = PFD_MAIN_PLANE;

    int SuggestedPixelFormatIndex = ChoosePixelFormat(WindowDC, &DesiredPixelFormat);
    PIXELFORMATDESCRIPTOR SuggestedPixelFormat;
    DescribePixelFormat(WindowDC, SuggestedPixelFormatIndex,
                        sizeof(SuggestedPixelFormat), &SuggestedPixelFormat);
    SetPixelFormat(WindowDC, SuggestedPixelFormatIndex, &SuggestedPixelFormat);
    
    HGLRC OpenGLRC = wglCreateContext(WindowDC);
    if(wglMakeCurrent(WindowDC, OpenGLRC))        
    {
        // NOTE(casey): Success!!!
    }
    else
    {
        InvalidCodePath;
        // TODO(casey): Diagnostic
    }
    ReleaseDC(Window, WindowDC);
}

internal void
Win32DisplayBufferInWindow(win32_offscreen_buffer *Buffer,
                           HDC DeviceContext, int WindowWidth, int WindowHeight)
{
    glViewport(0, 0, WindowWidth, WindowHeight);
    glClearColor(1.0f, 0.0f, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    SwapBuffers(DeviceContext);
}
```

Blending
```C

glEnable(GL_BLEND);
glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

// non premultiplied equation
DestColor = SourceAlpha * SourceColor + (1 - SourceAlpha) * DestColor;

// premultiplied equation
DestColor = SourceAlpha + (1 - SourceAlpha) * DestColor;
```

glTexParameteri(GL_TEXTURE_MIN_FILTER/GL_TEXTURE_MAG_FILTER, ...) - G GL_NEAREST повторяет цвет пикселя на ближайшие пиксели
glTexParameteri(GL_TEXTURE_MIN_FILTER/GL_TEXTURE_MAG_FILTER, ...) - GL_LINEAR смешивает цвет ближайших пикселей

sRGB - colorspace (gamma curve)

VSync - необходима, иначе во время отрисовки нового кадра будет видна часть прошлого
wglSwapInterval(...);

TLB - Translation lookaside buffer

### Matrices

TransformedVector = TranslationMatrix * RotationMatrix * ScaleMatrix * OriginalVector;

В C++, с GLM:
glm::mat4 myModelMatrix = myTranslationMatrix * myRotationMatrix * myScaleMatrix;
glm::vec4 myTransformedVector = myModelMatrix * myOriginalVector;

В GLSL:
mat4 transform = mat2 * mat1;
vec4 out_vec = transform * in_vec;

## Seamless ground rendering

Ground Chunks отрисовываются красным цветом - обход чанков в области камеры и их отрисовка
<p align="center"><img src="https://i.imgur.com/fk6Osqa.png" width="1000">
Ground Chunks (вид сверху)</p>

Seamless Ground Texture
<p align="center"><img src="https://i.imgur.com/m0dVLfg.png" width="1000">
Seamless Ground Texture (вид сверху)</p>

Seamless Ground Model (GL_LINES)
<p align="center"><img src="https://i.imgur.com/66SAlev.png" width="1000">
Not Filled Seamless Ground Model (вид сверху)</p>

Seamless Ground Model (GL_LINES)
<p align="center"><img src="https://i.imgur.com/mNnF25d.png" width="1000">
Not Filled Seamless Ground Model (вид c камеры игрока)</p>

Seamless Ground Model (GL_TRIAGNLES + TEXTURE)
<p align="center"><img src="https://i.imgur.com/ScSqF2k.png" width="1000">
Filled Seamless Ground Model (вид с камеры игрока)</p>

## 2d

Rotation and Scaling Coordinate System in 2d
```C
r32 Angle = 45.0f;
r32 Scale = 100.0f;
v2 XAxis = Scale*V2(Cos(Angle), Sin(Angle));
v2 YAxis = Scale*V2(-XAxis.y, XAxis.x));
```

## Rays and Spheres

Уравнение окружности
```
r^{2}=x^{2}+y^{2}`
```
y=\sqrt{4-x^{2}} - первая половина окружности
y=-\sqrt{4-x^{2}} - вторая половина окружности

Векторая форма уравнения окружности
```
(C−P)⋅(C−P)=r2
```

Функция луча
```
Pxy = Axy + BxyT, где
A - исходная точка (x, y)
B - направление перемещения, например (1, 1)
T - дистанция перемещения
```

Parametic equations
```
Px = Ax + BxT
Py = Ay + ByT
```

После подстановки исходной точки и направления в уравнение получим квадратное уравнение с неизвестной T
```
(Ax + BxT)^2 + (Ay + ByT)^2 - r^2 = 
Ax^2 + 2AxBxT + Bx^2T^2 + 2AyByT + By^2T^2 - r^2 = 
(Bx^2+By^2)T^2 + (2AxBx + 2AyBy)T + (Ax^2 + Bx^2 - r^2
```

Quadratic formula
```
(-b +-sqrt(b^2 - 4ac))/2a
```
>
\>0 - 2 solutions
=0 - 1 solution
<0 - 0solutions

Решения квадратного уравнения и координаты точек прямой нужно подставить в Pxy = Axy + BxyT и получим позицию пересечения прямой с окружностью

### Ray tracing, Ray casting, Ray marching

Трассировка лучей (ray tracing), рейкастинг (ray casting) и реймэрчинг (ray marching) - это методы визуализации, которые используются для создания 2D и 3D изображений, но они отличаются способом расчета пересечения лучей с объектами.
Вкратце:
Трассировка лучей: точная, но медленная, для фотореалистичных изображений.
Рейкастинг: быстрый, но менее точный, для игровых сцен.
Реймэрчинг: компромисс между скоростью и точностью, для сложных 3D сцен. 

#### Трассировка лучей (ray tracing)

Трассировка лучей вычисляет точное пересечение луча с геометрией сцены, используя математические вычисления для прямого перехода к объекту, что обеспечивает высокое качество изображения. 
Этот метод очень точный, но вычислительно дорогой, особенно для сложных сцен, так как необходимо просчитывать множество лучей. 
Трассировка лучей часто применяется для создания фотореалистичных изображений в профессиональной графике и в играх, где требуется высокое качество, но не важна скорость.

#### Рейкастинг (ray casting)

Рейкастинг работает путем итеративного продвижения луча по направлению к объекту, шаг за шагом, пока он не столкнется с ним. 
Этот метод проще и быстрее, чем трассировка лучей, но может приводить к менее точным результатам, особенно при работе с криволинейными поверхностями. 
Рейкастинг часто используется в 3D играх для создания быстрых, но не очень качественных изображений. 

#### Реймэрчинг (ray marching)

Реймэрчинг, как и рейкастинг, работает итеративно, но вместо простого шага, он использует функцию расстояния (SDF), чтобы определить, насколько далеко луч продвинулся от объекта. 
Это позволяет более эффективно обрабатывать сложные поверхности и рассчитывать нормали для освещения, что делает его полезным для создания фракталов и других сложных 3D сцен. 
Реймэрчинг может быть более быстрым и точным, чем рейкастинг, но требует использования SDF, что может быть сложным для некоторых объектов. 

### Examples

#### Ray tracing shader 1

```C
v3 RayOrigin = V3(0, 0, 2);
v3 RayDirection = V3(Coord.x, Coord.y, -1.0f);

r32 a = Inner(RayDirection, RayDirection);
r32 b = 2.0f * Inner(RayOrigin, RayDirection);
r32 c = Inner(RayOrigin, RayOrigin) - Radius * Radius;
r32 Discriminant = b * b - 4.0f * a * c;
if(Discriminant >= 0.0f)
{
	RayColor = V4(1, 0, 1, 1.0f);
}
```

#### Ray tracing shader 2

```C
    // Test ray cast
    FragColor = vec4(0, 0, 0, 1);
    vec3 LightDir = vec3(-1, -1, 1);
    vec3 SphereOrigin = vec3(0, 0, 0);
    vec3 RayOrigin = vec3(0, 0, 1);
    vec2 Coord = vec2(gl_FragCoord.x / 1920, gl_FragCoord.y / 1080);
    Coord = Coord * 2.0f - 1.0f;
    Coord.x = Coord.x * 1920.0 / 1080.0;

    vec3 RayDirection = vec3(Coord.x, Coord.y, -1.0f);
    float Radius = 0.5f;

    float a = dot(RayDirection, RayDirection);
    float b = 2.0f * dot(RayOrigin, RayDirection);
    float c = dot(RayOrigin, RayOrigin) - Radius * Radius;
    float Discriminant = b * b - 4.0f * a * c;
    if(Discriminant >= 0.0f)
    {
        float FurthestT = -b + sqrt(Discriminant) / (2.0f * a);
        float ClosestT = -b - sqrt(Discriminant) / (2.0f * a);
        
        //FragColor = vec4(1, 0, 1, 1.0f);
        
    /*         vec3 HitPosition1 = RayOrigin + RayDirection * t1;
        vec3 Normal1 = HitPosition1 - SphereOrigin;
        Normal1 = normalize(Normal1);
        float Light1 = max(dot(Normal1, -LightDir), 0.0f);
        vec3 Color = SphereColor * Light1; */
        
    /*         vec3 HitPosition2 = RayOrigin + RayDirection * t2;
        vec3 Normal2 = HitPosition2 - SphereOrigin;
        Normal2 = normalize(Normal2);
        float Light2 = max(dot(Normal2, -LightDir), 0.0f);
        vec3 Color = SphereColor * Light2;
        FragColor = vec4(Color, 1.0f);
        */
        
        vec3 HitPoint = RayOrigin + RayDirection * ClosestT;
        vec3 Normal = normalize(HitPoint);
        
        float d = max(dot(Normal, -LightDir), 0.0f); // cos(angle), cos(> 90) == -1
        
        // vec3 Color = Normal * 0.5 + 0.5;
        vec3 Color = vec3(1, 0, 1);
        Color *= d;
        FragColor = vec4(Color, 1.0f);
    }
```

#### Ray tracing shader 3

```C
struct sphere
{
  vec3 Position;
  float Radius;
  vec3 Albedo;
};

struct ray
{
    vec3 Origin;
    vec3 Direction;
};

struct trace_ray_result
{
    int ClosestSphereIndex;
    float HitDistance;
};

struct closest_hit
{
    float HitDistance;
    vec3 WorldPosition;
    vec3 WorldNormal;
    int Index;
};

#define FLT_MAX 3.402823466e+38
sphere SphereArray[2];

trace_ray_result TraceRay(ray Ray)
{
    trace_ray_result Result;
    Result.ClosestSphereIndex = -1;
    Result.HitDistance = FLT_MAX;
    for (int i = 0; i < SphereArray.length(); i++)
    {
        vec3 Origin = Ray.Origin - SphereArray[i].Position;

        float a = dot(Ray.Direction, Ray.Direction);
        float b = 2.0f * dot(Origin, Ray.Direction);
        float c = dot(Origin, Origin) - SphereArray[i].Radius * SphereArray[i].Radius;
        float Discriminant = b * b - 4.0f * a * c;
        if(Discriminant >= 0.0f)
        {
            float ClosestT = -b - sqrt(Discriminant) / (2.0f * a);
            //float FurthestT = -b + sqrt(Discriminant) / (2.0f * a);
            if(ClosestT > 0.0f && ClosestT < Result.HitDistance)
            {
                Result.HitDistance = ClosestT;
                Result.ClosestSphereIndex = i;
            }
        }
    }
    return(Result);
}

closest_hit ClosestHit(ray Ray, float HitDistance, int ClosestSphereIndex)
{
    closest_hit Result;
    Result.HitDistance = HitDistance;
    Result.Index = ClosestSphereIndex;
    
    vec3 Origin = Ray.Origin - SphereArray[ClosestSphereIndex].Position;
    Result.WorldPosition = Origin + Ray.Direction * HitDistance;
    Result.WorldNormal = normalize(Result.WorldPosition);
    
    Result.WorldPosition += SphereArray[ClosestSphereIndex].Position;
    
    return(Result);
}

void mainImage( out vec4 FragColor, in vec2 FragCoord )
{
    vec2 Coord = vec2(FragCoord.x / iResolution.x, FragCoord.y / iResolution.y);
    Coord = Coord * 2.0f - 1.0f;
    Coord.x = Coord.x * iResolution.x / iResolution.y;

    SphereArray[0] = sphere(vec3(0, 0, 0), 0.5, vec3(1, 0, 1));
    SphereArray[1] = sphere(vec3(0, -104, 0), 100.0, vec3(0.2, 0.3, 1));
    ray Ray = ray(vec3(0, 0, -1), vec3(Coord.x, Coord.y, 1.0f));

    int BounceCount = 2;
    vec3 Color = vec3(0);
    float Multiplier = 1.0;
    for(int i = 0; i < BounceCount; i++)
    {
        trace_ray_result TraceRayResult = TraceRay(Ray);
        if(TraceRayResult.ClosestSphereIndex < 0)
        {
            vec3 SkyColor = vec3(0, 0, 0);
            Color += SkyColor * Multiplier;
            break;
        }
        else
        {
            closest_hit PayLoad = ClosestHit(Ray, TraceRayResult.HitDistance, TraceRayResult.ClosestSphereIndex);

            vec3 LightDir = vec3(-1, -1, -1);
            float LightIntensivity = max(dot(PayLoad.WorldNormal, -LightDir), 0.0f); // cos(angle), cos(> 90) == -1

            vec3 SphereColor = SphereArray[PayLoad.Index].Albedo;
            SphereColor *= LightIntensivity;
            Color += SphereColor * Multiplier;
            Multiplier *= 0.7;
            
            Ray.Origin = PayLoad.WorldPosition + PayLoad.WorldNormal * 0.0001;
            Ray.Direction = reflect(Ray.Direction, PayLoad.WorldNormal);
        }
    }
    FragColor = vec4(Color, 1.0f);
}
```

#### Ray tracing 4

```C
float sphere(vec3 ray, vec3 dir, vec3 center, float radius)
{
	vec3 rc = ray-center;
	float c = dot(rc, rc) - (radius*radius);
	float b = dot(dir, rc);
	float d = b*b - c;
	float t = -b - sqrt(abs(d));
	float st = step(0.0, min(t,d));
	return mix(-1.0, t, st);
}

vec3 background(float t, vec3 rd)
{
	vec3 light = normalize(vec3(sin(t), 0.6, cos(t)));
	float sun = max(0.0, dot(rd, light));
	float sky = max(0.0, dot(rd, vec3(0.0, 1.0, 0.0)));
	float ground = max(0.0, -dot(rd, vec3(0.0, 1.0, 0.0)));
	return 
		(pow(sun, 256.0)+0.2*pow(sun, 2.0))*vec3(2.0, 1.6, 1.0) +
		pow(ground, 0.5)*vec3(0.4, 0.3, 0.2) +
		pow(sky, 1.0)*vec3(0.5, 0.6, 0.7);
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
	vec2 uv = (-1.0 + 2.0*fragCoord.xy / iResolution.xy) * 
		vec2(iResolution.x/iResolution.y, 1.0);
	vec3 ro = vec3(0.0, 0.0, -3.0);
	vec3 rd = normalize(vec3(uv, 1.0));
	vec3 p = vec3(0.0, 0.0, 0.0);
	float t = sphere(ro, rd, p, 1.0);
	vec3 nml = normalize(p - (ro+rd*t));
	vec3 bgCol = background(iTime, rd);
	rd = reflect(rd, nml);
	vec3 col = background(iTime, rd) * vec3(0.9, 0.8, 1.0);
	fragColor = vec4( mix(bgCol, col, step(0.0, t)), 1.0 );
}
```


#### Ray tracing 5

Shadertoy
```C
float Width = 800.0;
float Height = 450.0;
vec3 RayOrigin = vec3(0, 0, 1);
float SphereRadius = 0.6;
vec3 SphereCenter = vec3(0, 0, 0);
vec3 LightDir = vec3(-1, -1, -1);

float HitSphere(vec3 Center, float Radius, vec3 RayDirection)
{
    vec3 OC = Center - RayOrigin;
    float a = dot(RayDirection, RayDirection);
    float b = -2.0f * dot(RayDirection, OC);
    float c = dot(OC, OC) - Radius * Radius;
    float Discriminant = b * b - 4.0f * a * c;
    float Result = 0.0;
    if(Discriminant < 0.0)
    {
        Result = -1.0;
    }
    else
    {
        Result = (-b - sqrt(Discriminant)) / 2.0 * a;
    }
    return(Result);
}

void mainImage(out vec4 FragColor, in vec2 FragCoord)
{
    vec2 Coord = vec2(FragCoord.x / Width, FragCoord.y / Height);
    Coord = Coord * 2.0f - 1.0f;
    Coord.x = Coord.x * Width / Height;
    vec3 RayDirection = vec3(Coord.x, Coord.y, -1.0f);
    float t = HitSphere(SphereCenter, SphereRadius, RayDirection);
    if(t > 0.0)
    {
        vec3 Normal = (RayOrigin + t * RayDirection) - vec3(0, 0, -1);
        vec3 Color = 0.5 * vec3(Normal.x + 1.0, Normal.y + 1.0, Normal.z + 1.0);
        
        float d = max(dot(Normal, -LightDir), 0.0f); // cos(angle), cos(> 90) == -1
        //Color = vec3(1, 0, 0);
        Color *= d;
        
        FragColor = vec4(Color, 1.0f);
    }
    else
    {
        FragColor = vec4(0.0, 0.0, 1.0, 1.0f);
    }
}
```