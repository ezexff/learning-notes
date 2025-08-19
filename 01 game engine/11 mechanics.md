# Half-Life 2 movement

1. Получаем направление взгляда игрока
    ```
    Vector forward, right, up;
    AngleVectors(mv->m_vecViewAngles, &forward, &right, &up);
    ```
2. Получаем перемещение игрока от клавиш wasd 
    ```
    float fmove = mv->m_flForwardMove;
    float smove = mv->m_flSideMove;
    ```
3. Формируем желаемую скорость
    ```
    Vector wishvel;
    for (i=0 ; i<2 ; i++)
		wishvel[i] = forward[i]*fmove + right[i]*smove;
    wishvel[2] = 0;
    ```
4. Нормализация вектора (определение величины скорости движения?)
    ```
    VectorCopy(wishvel, wishdir);
    float wishspeed = VectorNormalize(wishdir);
    ```
5. Установка скорости перемещения
    ```
    mv->m_vecVelocity[2] = 0;
    Accelerate(wishdir, wishspeed, sv_accelerate.GetFloat());
    mv->m_vecVelocity[2] = 0;
    ```
6. Добавьте любую базовую скорость к текущей скорости
    VectorAdd(mv->m_vecVelocity, player->GetBaseVelocity(), mv->m_vecVelocity);
    float spd = VectorLength(mv->m_vecVelocity);
    
7. Первая попытка перемещения
    dest[0] = mv->GetAbsOrigin()[0] + mv->m_vecVelocity[0]*gpGlobals->frametime;
    dest[1] = mv->GetAbsOrigin()[1] + mv->m_vecVelocity[1]*gpGlobals->frametime;	
    dest[2] = mv->GetAbsOrigin()[2];