# World

**Игровой мир** - огромная трёхмерная область, которая состоит из x*y*z (int32 или 4 миллиарда) чанков

**Чанк** - маленький кусочек игрового мира, представленный в виде куба размером x*y*z (int32 или 4 миллиарда) метров

**Сущность** - объект игрового мира (игрок, стена, монстр и т.д.)

Мировые координаты сущности
* v3 ChunkP позиция чанка
* v3 Offset смещение на чанке относительно его центра

**Sparse World Storage** - мир в постоянной памяти представлен в виде связного списка, содержащего информацию только о чанках на которых сущности. Это намного эффективнее, чем создавать двумерный или трёхмерный массив. Во втором случае, даже если чанк пустой (на нём нет сущностей), то будет занимать место в памяти

Игровой мир со сплошной геометрией (constructive solid geometry, реализован в Quake), т.е. сущности могут находиться, перемещаться и т.д. лишь внутри сущности Space. Данный подход более надёжный, но менее эффективный, чем противоположный (реализован в Unreal), когда сущности могут находиться в любой точке мирового пространства. Во втором случае, например, часто возникают дыры (leaks). В нашем случае надёжность > эффективность

Из-за физических пределов компьютеров невозможно хранить в оперативной памяти и обрабатывать огромный мир целиком. Чтобы решить эту проблему можно использовать симулируемый регион (SimRegion)

**SimRegion** - куб в центре которого камера/игрок

**Ground** - понятие, относительно которого рассчитываются высоты объектов и их положения в пространстве

#### Одна итерация симулируемого региона
* Создаём список сущностей, которые попали в границы (Bounds) симулируемой области и загружаем информацию о них из постоянной (Permanent) памяти во временную (Transient)
* В чуть меньших границах (UpdatableBounds) обрабатываем у сущностей мувмент и коллизии. Дополнительные границы нужны для того, чтобы избежать проблем с коллизиями при перемещении сущности из SimRegion в память
* Сохраняем в постоянной памяти новую информацию о сущностях

#### 2 состояния сущности
* в хранилище или же low_entity (вне симулируемой области)
* симулируемая или же sim_entity (внутри симулируемой области)

Для обращения к сущностям используется HashMap

#### Реализация SimRegion выполняется при помощи методов `BeginSim()` и `EndSim()`
* BeginSim()
  * определяем чанки находящиеся в симулируемой области
  * обходим чанки в симулируемой области
    * обходим блоки с сущностями
      *  обходим сущности из хранилища (low_entity)
        * определяем позицию сущности в системе координат симулируемой области
        * добавляем сущность из хранилища в симулируемые (sim_entity)
* EndSim()
  * обходим симулируемые сущности
    * присваиваем симулируемым сущностям в хранилище новую позицию (после просчёта физики)

В SimRegion просчёты физики выполняются относительно позиции камеры/игрока, т.е при просчёте физики позиция камеры/игрока это v3(0,0,0), что позволяет
* не выполнять конвертацию позиций в мировую систему координат
* избежать float precision problem
* избежать проблем конвертации позиции сущности из мировой системы координат в локальную

#### SimRegion (вид сверху), где

<p align="center"><img src="https://i.imgur.com/BSsFvpx.png" width="1000"></p>

* Квадрат жёлтого цвета - границы камеры: CameraBoundsInMeters = SimChunksInCamera * World->ChunkDimInMeters
* Квадрат лазурного цвета - начальные границы симуляции: SimBounds = CameraBoundsInMeters + SimBoundsExpansion
* Квадрат пурпурного цвета - границы обновления сущностей внутри симулируемого региона: SimRegion->UpdatableBounds = CameraBoundsInMeters + SimBoundsExpansion + SimRegion->MaxEntityRadius
* Квадрат оранжевого цвета - границы добавления чанков с сущностями в симулируемый регион: SimRegion->Bounds = SimRegion->UpdatableBounds + UpdateSafetyMargin(SimRegion->MaxEntityRadius + dt * SimRegion->MaxEntityVelocity)
* Квадраты зелёного цвета - чанки, обрабатываемые в симулируемом регионе: World->ChunkDimInMeters
* Прямоугольники синего цвета - пространство, где сущности могут перемещаться


## Entity

The Sparse Entity System - одна большая энтити с огромным числом параметров (без типов энтити и т.д.), поведение сущности и рендеринг зависят лишь от параметров этой энтити, а не эт её типа

## Movement

#### Consistant physics
```C
// NOTE(casey): Clamp the dt for the frame so that we don't have too high
// or too low frame rates in any circumstance.
if(Input->dtForFrame > 0.1f)
{
    Input->dtForFrame = 0.1f;
    // TODO(casey): Warn on out-of-range refresh
}
else if(Input->dtForFrame < 0.001f)
{
    Input->dtForFrame = 0.001f;
    // TODO(casey): Warn on out-of-range refresh
}
```

#### EOM (Equation of motion)
```
Rigid body dynamics (Динамика жесткого тела):
F = d/dt (mv)

Physics (Movement):
Position = f(t), Velocity = f'(t), Acceleration = f"(t)

Example Movement Equation:
x  = f(t)  = 5t^2 + 2t + 3 (meters) or (m)
x' = f'(t) = 10t + 2 (meters/seconds) or (m/s)
x" = f"(t) = 10 = a (meters/seconds^2) or (m/s^2)

Example Default Movement Equation with variables
(a - acceleration, v - const old velocity, p - const old Position):
x = f(t) = 5t^2 + vt + 3 (where 3 is p - old position) = 
= (a/2)*t^2 + vt + p x' = f'(t) = 
= at + 2 (where 2 is v - old velocity) = 
= at + v x" = f"(t) = 
= 10 = a

We can get old position with Δt = 0
f(0)  = p
f'(0) = v
f"(0) = a

New Movement Equation (Derivative)
f(t)  = 1/2*a*t^2 + vt + f(0)
f'(t) = at + v
f"(t) = a

New Movement Equation (Code Integration):
x =  p' = (a/2)*t^2 + vt + p (new position)
x' = v' = at + v (new velocity)
x" = a  = a
```

#### Vectors (reflection and gliding)
```
Vectors (reflection & gliding)
v' = refl velocity vector
v = velocity vector
r = wall perpendicular unit vector (wall vector length = 1)
Inner = Dot product
v' = v - 2 * Inner(v, r) * r

2 change to 1 in equation for projection vector on wall (gliding)
v' = gliding velocity vector
v' = v - 1 * Inner(v, r) * r
```

## Collision detection

Обнаружение столкновения игрока и стены это обнаружение точки пересечения двух линий. Первая линия это расстояние которое может пройти игрок с текущими скоростью и ускорением, а вторая линия это стена

Два подхода к реализации обнаружения столкновений **Search in P** против **Search in T**

**Search in P** (поиск по позиции)
* +bounded in iterations
* +smoothly ignoring geometry bugs
* -search in p
* -don't handles bounciness 

**Search in T** (поиск во времени)
* +handles bounciness better
* +never need to form the space
* -unbounded in iterations
* -gets stuck on geometry bugs

Collision detection is an o(n^2) problem

Во время обнаружения столкновений нужно ответить на 2 вопроса
* когда мы сталкиваемся со стеной
* что делать, когда сталкиваемся со стеной

Для определение координаты столкновения используем уравнение сглаживания вида  
`A + t * (B - A)`, где `(B - A) = d` - расстояние от A до B

Уравнение для определения позиции во времени  
`P(t) = P0 + t * D`, где  
* P(t) - позиция во времени  
* P0 - начальная позиция  
* t - время [0, 1], в промежутке от 0 до 1  
* D - расстояние, которое необходимо пройти  

Пример уравнения для координаты x  
`P(t)x = P0x + t * Dx = Wx`, где  
* P(t)x - позиция во времени для x  
* P0x - начальная позиция x  
* t - время [0, 1], в промежутке от 0 до 1  
* Dx - расстояние, которое необходимо пройти х  
* Wx - x стены (wall x)  

Уравнения для бесконечных вертикальных и горизонтальных стен  
`t = (Wx - P0x) / Dx` и `t = (Wy - P0y) / Dy`, где

Dx должно быть != 0, потому что деление на 0, но, в нашем случае, это имеет значение. Деление на 0, в данном случае, сделает луч параллельным оси Y, следовательно столкновения игрока и стены никогда не случится. В таком случае нет смысла проверять на столкновение игрока и стену, т.к. этого не произойдёт `t >= 0`

Если подставить уравнение `ts = (Wx - P0x) / Dx`, где tS - t solved, в уравнение `P(t) = P0 + t * D` вместо t, то получим исправленное уравнения (Ps - Position solved) для конечных вертикальных и горизонтальных стен  
`Ps = P0 + ts * D`

Этим уравнением получаем Ps позицию в которой происходит столкновение со стеной по x координате. Зная одну точку, в которой происходит столкновение, мы можем проверить входит ли y координата в заданный диапазон (минимум и максимум y координаты стены), что даёт уравнения для конечных вертикальных и горизонтальных стен

Пересечение прямоугольников - два прямоугольника пересекаются, если имеют хотя бы одну общую точку
```
     min  A  max     min  B  max
- - - * - - - * - - - * - - - * - - -
```

* Если min A > max B - не пересекаются
* Если max A < min B - не пересекаются
* В остальных случаях  - пересекаются

<p align="center"><img src="https://i.imgur.com/GjVyA1U.png" width="500"></p>

Δt - расстояние, на которое должен переместиться игрок, учитывая текущие скорость и ускорение. Например, при нормализованном значении `Δt = 1`, если столкновение происходит на `Δt1 = 0.3`, то остаточное расстояние `Δt2 = Δt - Δt1 = 1 - 0.3 = 0.7` можно использовать для расчёта скольжения вдоль стены. Если мы знаем, когда сталкиваемся со стеной, то можем обновить вектор скорости и направить его вдоль стены для создания эффекта скольжения

<p align="center"><img align="center" src="https://i.imgur.com/29RB375.png" width="500"></p>

Для более точной обработки скольжения можно выполнять 4 итерации. На каждой из итераций используем скалярное произведение, чтобы выровнять вектор скорости вдоль стены

При обнаружении столкновений лучом из точки позиции игрока возникает проблема: игрок может проходить между прямоугольниками, промежуток которых меньше диаметра игрока

Чтобы данной проблемы не возникло, необходимо, при обнаружении столкновений лучом, учитывать диаметр игрока. Мы обводим прямоугольником игрока прямоугольник стены и для каждой стены получаем новые фигуры

<p align="center"><img align="center" src="https://i.imgur.com/UCnVcj8.png" width="500"></p>

Теперь при выполнении рейкаста из точки позиции игрока будет учитываться диаметр игрока

<p align="center"><img align="center" src="https://i.imgur.com/zgUN936.png" width="250"></p>

Относительно позиции сущности можно определить набор прямоугольников или кубов (sim_entity_collision_volume) с каждым из которых будут проверяться столкновения. Это позволит обрабатывать столкновения более сложной формы.  Один уровень коллизии это v3 OffsetP (позиция) и v3 Dim (размер параллелепипеда)

Сумма Минковского - скаладываем два прямоугольника и получаем новую фигуру. Обводим одной фигурой по границе другой. Это позволит выполнять проверку на столкновения лучом с учётом размера игрока (прямоугольника)
```C
v3 MinkowskiDiameter = {TestVolume->Dim.x + Volume->Dim.x,
                        TestVolume->Dim.y + Volume->Dim.y,
                        TestVolume->Dim.z + Volume->Dim.z};
v3 MinCorner = -0.5f*MinkowskiDiameter;
v3 MaxCorner = 0.5f*MinkowskiDiameter;
```

Если разность позиций прямоугольников меньше диаметра общей фигуры, то прямоугольники пересекаются

Для реализации игрового мира со сплошной геометрией необходимо добавить новую переменную tMax. Если tMin изменяется от 0 до 1, то tMax от 1 до 0
* tMin - переменная, останавливающая игрока при столкновении с ближайшей стеной
* tMax - переменная, позволяющая игроку двигаться в пределах сущности Space. Проверка столкновения с дальней точкой Space

Помимо новой переменной необходимо добавить флаг Traversable для сущности Space, по которому система столкновений будет понимать, что внутри Space игрок может двигаться

Относительная позиция игрока по отношению к стене. Когда игрок дойдёт до стены, то эта точка станет [0, 0, 0]
```C
v3 Rel = ((Entity->P + Volume->OffsetP) - (TestEntity->P + TestVolume->OffsetP));
```

Пример обнаружения столкновения игрока со стеной, где Entity это игрок, а TestEntity это стена
```C
struct test_wall
{
    real32 X; // точка, где расположена линия для стороны стены
    real32 RelX; // x расстояние игрока до стены
    real32 RelY; // y расстояние игрока до стены
    real32 DeltaX; // x расстояние, на которое хочет передвинуться игрок
    real32 DeltaY; // y расстояние, на которое хочет передвинуться игрок
    real32 MinY; // минимальная граница линии для стороны стены
    real32 MaxY; // максимальная граница линии для стороны стены
    v3 Normal; // нормаль для расчёта скольжения вдоль стены
};
test_wall Walls[] = {
    {
        MinCorner.x, Rel.x, Rel.y, 
        PlayerDelta.x, PlayerDelta.y, 
        MinCorner.y, MaxCorner.y, 
        V3(-1, 0, 0)
    },
    {
        MaxCorner.x, Rel.x, Rel.y, 
        PlayerDelta.x, PlayerDelta.y, 
        MinCorner.y, MaxCorner.y, 
        V3(1, 0, 0)
    },
    {
        MinCorner.y, Rel.y, Rel.x, 
        PlayerDelta.y, PlayerDelta.x, 
        MinCorner.x, MaxCorner.x, 
        V3(0, -1, 0)
    },
    {
        MaxCorner.y, Rel.y, Rel.x, 
        PlayerDelta.y, PlayerDelta.x, 
        MinCorner.x, MaxCorner.x, 
        V3(0, 1, 0)
    },
};

real32 tMin = 1.0f; // мультипкликатор движения
real32 tMax = 0.0f;

real32 tMinTest = tMin;
bool32 HitThis = false;

v3 TestWallNormal = {};
for(uint32 WallIndex = 0;
    WallIndex < ArrayCount(Walls);
    ++WallIndex)
{
    test_wall *Wall = Walls + WallIndex;

    real32 tEpsilon = 0.001f; // безопасное расстояние, на которое игрок может подойти к стене

    if(Wall->DeltaX != 0.0f) // игрок движется
    {
        // насколько по первой координате игрок близок к стене
        real32 tResult = (Wall->X - Wall->RelX) / Wall->DeltaX;

        // насколько по второй координате игрок близок к стене
        real32 Y = Wall->RelY + tResult * Wall->DeltaY;

        // (tResult >= 0.0f) проверяем направлен ли луч движения игрока сторону стены
        // (tMinTest > tResult) проверяем по первой координате подошёл ли игрок к стене
        if((tResult >= 0.0f) && (tMinTest > tResult))
        {
            // проверяем по второй координате попадает ли луч игрока в диапазон линии стены
            if((Y >= Wall->MinY) && (Y <= Wall->MaxY))
            {
                // столкновение
                // обновляем мультипликатор движения
                tMinTest = Maximum(0.0f, tResult - tEpsilon);
                TestWallNormal = Wall->Normal;
                HitThis = true;
            }
        }
    }
}
```

## Particles

Eulerian sim method vs Lagrangian sim method

**Lagrangian** - Simulation of the motion of particles through space (частицы занимают определённую область вокруг точки симуляции, положение которой определено относительной системой координат, из-за чего сложно определить плотность частиц)

**Eulerian** - Simulation of density and mean velociy of particles at specific locations in space (область существования частик это сетка, в клетках которой имеется параметр плотности - чем больше частиц в клетке, тем больше плотность клетки, это позволяет равномерно заполнять сетку частицами)

Пример
```C
struct hero_bitmap_ids
{
    bitmap_id Head;
    bitmap_id Cape;
    bitmap_id Torso;
};

struct particle_cel
{
    real32 Density;
    v3 VelocityTimesDensity;
};
struct particle
{
    bitmap_id BitmapID;
    v3 P;
    v3 dP;
    v3 ddP;
    v4 Color;
    v4 dColor;
};

for(u32 ParticleSpawnIndex = 0;
    ParticleSpawnIndex < 3;
    ++ParticleSpawnIndex)
{
    particle *Particle = GameState->Particles + GameState->NextParticle++;
    if(GameState->NextParticle >= ArrayCount(GameState->Particles))
    {
        GameState->NextParticle = 0;
    }

    Particle->P = V3(RandomBetween(&GameState->EffectsEntropy, -0.05f, 0.05f), 0, 0);
    Particle->dP = V3(RandomBetween(&GameState->EffectsEntropy, -0.01f, 0.01f), 7.0f*RandomBetween(&GameState->EffectsEntropy, 0.7f, 1.0f), 0.0f);
    Particle->ddP = V3(0.0f, -9.8f, 0.0f);
    Particle->Color = V4(RandomBetween(&GameState->EffectsEntropy, 0.75f, 1.0f),
                        RandomBetween(&GameState->EffectsEntropy, 0.75f, 1.0f),
                        RandomBetween(&GameState->EffectsEntropy, 0.75f, 1.0f),
                        1.0f);
    Particle->dColor = V4(0, 0, 0, -0.25f);
    Particle->BitmapID = GetRandomBitmapFrom(TranState->Assets, Asset_Head, &GameState->EffectsEntropy);
}

// NOTE(casey): Particle system test
ZeroStruct(GameState->ParticleCels);

r32 GridScale = 0.25f;
r32 InvGridScale = 1.0f / GridScale;
v3 GridOrigin = {-0.5f*GridScale*PARTICLE_CEL_DIM, 0.0f, 0.0f};
for(u32 ParticleIndex = 0;
    ParticleIndex < ArrayCount(GameState->Particles);
    ++ParticleIndex)
{
    particle *Particle = GameState->Particles + ParticleIndex;

    v3 P = InvGridScale*(Particle->P - GridOrigin);

    s32 X = TruncateReal32ToInt32(P.x);
    s32 Y = TruncateReal32ToInt32(P.y);

    if(X < 0) {X = 0;}
    if(X > (PARTICLE_CEL_DIM - 1)) {X = (PARTICLE_CEL_DIM - 1);}
    if(Y < 0) {Y = 0;}
    if(Y > (PARTICLE_CEL_DIM - 1)) {Y = (PARTICLE_CEL_DIM - 1);}

    particle_cel *Cel = &GameState->ParticleCels[Y][X];
    real32 Density = Particle->Color.a;
    Cel->Density += Density;
    Cel->VelocityTimesDensity += Density*Particle->dP;
}

#if 0
for(u32 Y = 0;
    Y < PARTICLE_CEL_DIM;
    ++Y)
{
    for(u32 X = 0;
        X < PARTICLE_CEL_DIM;
        ++X)
    {
        particle_cel *Cel = &GameState->ParticleCels[Y][X];
        real32 Alpha = Clamp01(0.1f*Cel->Density);
        PushRect(RenderGroup, GridScale*V3((r32)X, (r32)Y, 0) + GridOrigin, GridScale*V2(1.0f, 1.0f),
        V4(Alpha, Alpha, Alpha, 1.0f));
    }
}
#endif

for(u32 ParticleIndex = 0;
    ParticleIndex < ArrayCount(GameState->Particles);
    ++ParticleIndex)
{
    particle *Particle = GameState->Particles + ParticleIndex;

    v3 P = InvGridScale*(Particle->P - GridOrigin);

    s32 X = TruncateReal32ToInt32(P.x);
    s32 Y = TruncateReal32ToInt32(P.y);

    if(X < 1) {X = 1;}
    if(X > (PARTICLE_CEL_DIM - 2)) {X = (PARTICLE_CEL_DIM - 2);}
    if(Y < 1) {Y = 1;}
    if(Y > (PARTICLE_CEL_DIM - 2)) {Y = (PARTICLE_CEL_DIM - 2);}

    particle_cel *CelCenter = &GameState->ParticleCels[Y][X];
    particle_cel *CelLeft = &GameState->ParticleCels[Y][X - 1];
    particle_cel *CelRight = &GameState->ParticleCels[Y][X + 1];
    particle_cel *CelDown = &GameState->ParticleCels[Y - 1][X];
    particle_cel *CelUp = &GameState->ParticleCels[Y + 1][X];

    v3 Dispersion = {};
    real32 Dc = 1.0f;
    Dispersion += Dc*(CelCenter->Density - CelLeft->Density)*V3(-1.0f, 0.0f, 0.0f);
    Dispersion += Dc*(CelCenter->Density - CelRight->Density)*V3(1.0f, 0.0f, 0.0f);
    Dispersion += Dc*(CelCenter->Density - CelDown->Density)*V3(0.0f, -1.0f, 0.0f);
    Dispersion += Dc*(CelCenter->Density - CelUp->Density)*V3(0.0f, 1.0f, 0.0f);

    v3 ddP = Particle->ddP + Dispersion;

    // NOTE(casey): Simulate the particle forward in time
    Particle->P += (0.5f*Square(Input->dtForFrame)*Input->dtForFrame*ddP +
                    Input->dtForFrame*Particle->dP);
    Particle->dP += Input->dtForFrame*ddP;
    Particle->Color += Input->dtForFrame*Particle->dColor;

    if(Particle->P.y < 0.0f)
    {
        r32 CoefficientOfRestitution = 0.3f;
        r32 CoefficientOfFriction = 0.7f;
        Particle->P.y = -Particle->P.y;
        Particle->dP.y = -CoefficientOfRestitution*Particle->dP.y;
        Particle->dP.x = CoefficientOfFriction*Particle->dP.x;
    }

    // TODO(casey): Shouldn't we just clamp colors in the renderer??
    4 Color;
    Color.r = Clamp01(Particle->Color.r);
    Color.g = Clamp01(Particle->Color.g);
    Color.b = Clamp01(Particle->Color.b);
    Color.a = Clamp01(Particle->Color.a);

    if(Color.a > 0.9f)
    {
        Color.a = 0.9f*Clamp01MapToRange(1.0f, Color.a, 0.9f);
    }

    // NOTE(casey): Render the particle
    PushBitmap(RenderGroup, Particle->BitmapID, 1.0f, Particle->P, Color);
}
```