# Asset system

Очень много проблем возникает с именами файлов из-за несоответствия кодировок и поэтому отличной идеей будет архитектура позволяющая импортировать файлы с ассетами по типу файла. Игре не важно какие имена у файлов, а необходимо лишь их содержимое

2 components of Asset system
* Program for pack all game assets into big files (clean game directory)
* Code on game side that load and uses asset files

Push in render queue only if Bitmap/Texture asset valuable

Enum for types

Only load once asset in memory

AtomicCompareExchangeUInt32();

Tags for "pick best"

Asset Builder (program that pack game assets into one big file)

Example Asset Builder API
```C
BeginAssetsType(Assets, Asset_Grass);
AddBitmapAsset(Assets, "test2/grass00.bmp");
AddTag(Assets, FacingDirection, 0.0f);
AddBitmapAsset(Assets, "test2/grass01.bmp");
AddTag(Assets, FacingDirection, 2.0f);
AddBitmapAsset(Assets, "test2/grass02.bmp");
EndAssetType(Assets);
```

EAB FILE STRUCTURE
* Header
* Tags array
* AssetTypes array
* Assets array
* AssetSources array

EAB Header
<table>
    <tr>
        <th>struct name</th>
        <th>variable name</th>
        <th>struct size</th>
        <th>start pos in file </th>
    </tr>
    <tr>
        <td>eab_header</td>
        <td>Header</td>
        <td>44</td>
        <td>0</td>
    </tr>
    <tr>
        <td>eab_tag</td>
        <td>Assets->Tags</td>
        <td>8</td>
        <td>prev + sizeof(Header)</td>
    </tr>
    <tr>
        <td>eab_asset_type</td>
        <td>Assets->AssetTypes</td>
        <td>12</td>
        <td>prev + sizeof(tag) * Header.TagCount</td>
    </tr>
    <tr>
        <td>eab_asset</td>
        <td>Assets->Assets</td>
        <td>44</td>
        <td>prev + sizeof(eab_asset_type) * Header.AssetTypeCount</td>
    </tr>
    <tr>
        <td>asset_source</td>
        <td>Assets->AssetSources</td>
        <td>24</td>
        <td>prev + sizeof(eab_asset) * Header.AssetCount</td>
    </tr>
</table>

Asset file .EAB
* Header
  * MagicValue // check that we can load our asset file
  * Version
  * TagCount
  * AssetTypeCount // bitmap, sound and etc
  * AssetCount
  * Tags; // offset - where in file to get that array
  * AssetTypes; // offset -  where in file to get that array
  * Assets; // offset -  where in file to get that array
* Data blocks
  * Tags array
  * AssetTypes array
  * Assets array
  * AssetSources array

Loading .EAB file into game engine

Assets tree in engine after loading form .EAB file:
* Bitmaps (AssetType)
  * Clip (BitmapType)
    * Asset#1 (AssetSlot)
      * Width
      * Height
      * Tags
        * Opacity
* Sounds
  * Music
    * Asset#2 (AssetSlot)

Get asset by tag in our system
* Bitmap
  * Tag_Hero
    * Asset1
    * ...
    * AssetN
* Sound
  * TagMainMenuTheme
    * Asset1
    * ...
    * AssetN

Алгоритм импорта ассетов из .eab файлов в игру
* Формирование FileGroup для доступных *.eab файлов
* По FileGroup считываем заголовки (в заголовке содержится информация о положениях и числе элементов массивов, например, типы ассетов, теги и т.д.)
* По FileGroup считываем типы ассетов
* По FileGroup считываем теги
* По FileGroup считываем ассеты (циклы: типы ассетов->файлы->ассеты)
* Очищаем память выделенную для заголовков

Единый массив тегов - последовательно импортируем из всех *.eab файлов теги и помещаем друг за другом

Asset system состояния
* Unloaded - не в оперативной памяти, на жёстком диске
* Queued - поток начал загрузку с жёсткого диска в оперативную память
* Loaded - в оперативной памяти
* Locked - тред использует ассет, поэтому нужен запретит на его удаление/освобождение


