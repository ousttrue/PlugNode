# plugnode

node graph editor ?

## dependencies

* [lua-5.3.5(MIT License)](./dependencies/lua-5.3.5/doc/readme.html)
* [nlohmann/json(MIT License)](./dependencies/nlohmann_json/README.md)
* [plog(Mozilla Public License 2)](./dependencies/plog/LICENSE)
* imgui
* im3d

## ToDo

* [x] zoom focus
* [x] out drag
* [x] in click
* [x] zoom size
* [ ] eval graph
* [ ] save/load graph scene
* [ ] shader generator

## memo

```
   +---------------+
   |node           |
   +-------+-------+                        +-------+
pin|in slot|outslot|pin:src <--ref-- dst:pin|in slot|
   +-------+-------+                        +-------+
pin|in slot|       |
   +-------+-------+
```

* pin
  * type
  * value

* srcpin(入力): pin
  * setValue(value)
  * connect(dstpin)
  * weak_ptr: dstpins

* dstpin(出力): pin
  * getValue(): (is_new, value)
  * weak_ptr: src

* node(out, inout, in)
  * name
  * position
  * logic
    * process(in_slots, out_slots)

  * slot 値の表示・GUI(slider... etc)
    * name
    * position
    * size

    * out slot
      * setValue

* hover none / node / pin
* select pin / nodes

### 処理

1. NewFrame
2. OutNodeに値をセット
3. InNodeから逆向きにノードを辿って、深いところから値を解決する
