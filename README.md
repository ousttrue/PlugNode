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
* [x] eval graph
* [x] accept
* [x] remove node
* [ ] shader generator
* [x] type型
* [ ] graph is updated
* [ ] slot variation inheritance to member
* [ ] slot id
* [ ] context singleton
* [ ] save/load graph scene
* [ ] perilune::MetaMethod(value, pointer, shared_ptr, __tostring)

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
3. InNodeから逆向きにノードを辿る
