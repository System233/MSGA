<!--
 Copyright (c) 2022 System233
 
 This software is released under the MIT License.
 https://opensource.org/licenses/MIT
-->

# AMD64 指令编码方式

| REX  |   |   |   |   | Opcode  | ModRM |         |         | SIB   |         |         | Displacement | Immediate |
|------|---|---|---|---|---------|-------|---------|---------|-------|---------|---------|--------------|-----------|
| 0100b | W | R | X | B |         | mod   | reg     | r/m     | scale | index   | base    |              |           |
| 4    | 1 | 1 | 1 | 1 | 8/16/24 | 2     | REX.R+3 | REX.B+3 | 2     | REX.R+3 | REX.B+3 | 8/16/32      | 8/16/32   |

## REX

1.2.7 REX Prefix
1.4.4 Operand Addressing in 64-bit Mode

|字段|描述|
|-|-|
|REX|固定0100|
|REX.W|扩展到64位操作数宽度|
|REX.R|寄存器字段扩展,增加`ModRM.reg`到16个寄存器|
|REX.X|索引字段扩展,增加`SIB.index`到16个寄存器|
|REX.B|REX.B：基地址字段扩展,增加`ModRM.rm`、`SIB.base`和`opcode reg`到16个寄存器|

## ModRM

1.4.1 ModRM Byte Format

|字段|描述|
|-|-|
|ModRM.mod|11b:寄存器直接寻址,其他:寄存器间接寻址|
|ModRM.reg|扩展操作码或寄存器|
|ModRM.r/m|具体功能视ModRM.mod而定|

## SIB

1.4.2 SIB Byte Format

> effective_address = scale * index + base + offset

|字段|描述|
|-|-|
|SIB.scale|00b:1, 01b:2, 10b:4, 11b:8|
|SIB.index|索引寄存器|
|SIB.base|基地址寄存器|

### SIB.index与SIB.base编码

|编码|SIB.index|SIB.base|
|-|-|-|
|000|RAX|RAX|
|001|RCX|RCX|
|010|RDX|RDX|
|011|RBX|RBX|
|100| |RSP|
|101|RBP|RBP(mod=00b)|
|110|RSI|DH,RSI|
|111|RDI|BH,RDI|

### Mod.r/m=100b 时的SIB.base编码

|SIB|行为(mod!=11)|
|-|-|
|000|RAX|
|001|RCX|
|010|RDX|
|011|RBX|
|100|RSP|
|101|disp32 (mod=00b)<br>RBP+disp8 (mod=01)<br>RBP+disp32 (mod=10)<br>|
|110|RSI|
|111|RDI|

### ModRM与SIB编码




## 寄存器编号

2.3 Summary of Registers and Data Types

|编号|bin|高8位|低8位|16位|32位|64位|
|-|-|-|-|-|-|-|
0|0000|AH|AL|AX|EAX|RAX
1|0001|CH|CL|CX|ECX|RCX
2|0010|DH|DL|DX|EDX|RDX
3|0011|BH|BL|BX|EBX|RBX
4|0100||SPL|SP|SP|RSP
5|0101||BPL|BP|BP|RBP
6|0110||SIL|SI|SI|RSI
7|0111||DIL|DI|DI|RDI
8|1000||R8B|R8W|R8D|R8
9|1001||R9B|R9W|R9D|R9
10|1010||R10B|R10W|R10D|R10
11|1011||R11B|R11W|R11D|R11
12|1100||R12B|R12W|R12D|R12
13|1101||R13B|R13W|R13D|R13
14|1110||R14B|R14W|R14D|R14
15|1111||R15B|R15W|R15D|R15

## 操作码语法

* `/digit` 表示`ModRM.reg`为`digit`，只使用`ModRM.r/m`指定操作数，值`0-7`
* `/r` 表示在`ModRM`中同时指定`reg`和`r/m`
* `cb`,`cw`,`cd`,`cp`指定偏移值,分别为1,2,4,6个字节
* `ib`,`iw`,`id`,`iq`指定立即数,分别为1,2,4,6个字节
* `+rb`,`+rw`,`+rd`,`+rq`指定寄存器，分别1,2,4,8字节寄存器
* `m64` 64位内存操作数
* `+i` x87浮点堆栈操作数ST(i)


## JCC条件跳转指令表

P197 JCC Jump on Condition


|指令|Opcode|说明|
|-|-|-|
|`JO rel8off`<br>`JO rel16off`<br>`JO rel32off`|70 cb<br>0F 80 cw<br>0F 80 cd|Jump if overflow (`OF` = 1)|
|`JNO rel8off`<br>`JNO rel16off`<br>`JNO rel32off`|71 cb<br>0F 81 cw<br>0F 81 cd|Jump if not overflow (`OF` = 0)|
|`JB rel8off`<br>`JB rel16off`<br>`JB rel32off`|72 cb<br>0F 82 cw<br>0F 82 cd|Jump if below (`CF` = 1)|
|`JC rel8off`<br>`JC rel16off`<br>`JC rel32off`|72 cb<br>0F 82 cw<br>0F 82 cd|Jump if carry (`CF` = 1)|
|`JNAE rel8off`<br>`JNAE rel16off`<br>`JNAE rel32off`|72 cb<br>0F 82 cw<br>0F 82 cd|Jump if not above or equal (`CF` = 1)|
|`JNB rel8off`<br>`JNB rel16off`<br>`JNB rel32off`|73 cb<br>0F 83 cw<br>0F 83 cd|Jump if not below (`CF` = 0)|
|`JNC rel8off`<br>`JNC rel16off`<br>`JNC rel32off`|73 cb<br>0F 83 cw<br>0F 83 cd|Jump if not carry (`CF` = 0)|
|`JAE rel8off`<br>`JAE rel16off`<br>`JAE rel32off`|73 cb<br>0F 83 cw<br>0F 83 cd|Jump if above or equal (`CF` = 0)|
|`JZ rel8off`<br>`JZ rel16off`<br>`JZ rel32off`|74 cb<br>0F 84 cw<br>0F 84 cd|Jump if zero (`ZF` = 1)|
|`JE rel8off`<br>`JE rel16off`<br>`JE rel32off`|74 cb<br>0F 84 cw<br>0F 84 cd|Jump if equal (`ZF` = 1)|
|`JNZ rel8off`<br>`JNZ rel16off`<br>`JNZ rel32off`|75 cb<br>0F 85 cw<br>0F 85 cd|Jump if not zero (`ZF` = 0)|
|`JNE rel8off`<br>`JNE rel16off`<br>`JNE rel32off`|75 cb<br>0F 85 cw<br>0F 85 cd|Jump if not equal (`ZF` = 0)|
|`JBE rel8off`<br>`JBE rel16off`<br>`JBE rel32off`|76 cb<br>0F 86 cw<br>0F 86 cd|Jump if below or equal (`CF` = 1 or `ZF` = 1)|
|`JNA rel8off`<br>`JNA rel16off`<br>`JNA rel32off`|76 cb<br>0F 86 cw<br>0F 86 cd|Jump if not above (`CF` = 1 or `ZF` = 1)|
|`JNBE rel8off`<br>`JNBE rel16off`<br>`JNBE rel32off`|77 cb<br>0F 87 cw<br>0F 87 cd|Jump if not below or equal (`CF` = 0 and `ZF` = 0)|
|`JA rel8off`<br>`JA rel16off`<br>`JA rel32off`|77 cb<br>0F 87 cw<br>0F 87 cd|Jump if above (`CF` = 0 and `ZF` = 0)|
|`JS rel8off`<br>`JS rel16off`<br>`JS rel32off`|78 cb<br>0F 88 cw<br>0F 88 cd|Jump if sign (`SF` = 1)|
|`JNS rel8off`<br>`JNS rel16off`<br>`JNS rel32off`|79 cb<br>0F 89 cw<br>0F 89 cd|Jump if not sign (`SF` = 0)|
|`JP rel8off`<br>`JP rel16off`<br>`JP rel32off`|7A cb<br>0F 8A cw<br>0F 8A cd|Jump if parity (`PF` = 1)|
|`JPE rel8off`<br>`JPE rel16off`<br>`JPE rel32off`|7A cb<br>0F 8A cw<br>0F 8A cd|Jump if parity even (`PF` = 1)|
|`JNP rel8off`<br>`JNP rel16off`<br>`JNP rel32off`|7B cb<br>0F 8B cw<br>0F 8B cd|Jump if not parity (`PF` = 0)|
|`JPO rel8off`<br>`JPO rel16off`<br>`JPO rel32off`|7B cb<br>0F 8B cw<br>0F 8B cd|Jump if parity odd (`PF` = 0)|
|`JL rel8off`<br>`JL rel16off`<br>`JL rel32off`|7C cb<br>0F 8C cw<br>0F 8C cd|Jump if less (`SF` <> `OF`)|
|`JNGE rel8off`<br>`JNGE rel16off`<br>`JNGE rel32off`|7C cb<br>0F 8C cw<br>0F 8C cd|Jump if not greater or equal (`SF` <> `OF`)|
|`JNL rel8off`<br>`JNL rel16off`<br>`JNL rel32off`|7D cb<br>0F 8D cw<br>0F 8D cd|Jump if not less (`SF` = `OF`)|
|`JGE rel8off`<br>`JGE rel16off`<br>`JGE rel32off`|7D cb<br>0F 8D cw<br>0F 8D cd|Jump if greater or equal (`SF` = `OF`)|
|`JLE rel8off`<br>`JLE rel16off`<br>`JLE rel32off`|7E cb<br>0F 8E cw<br>0F 8E cd|Jump if less or equal (`ZF` = 1 or `SF` <> `OF`)|
|`JNG rel8off`<br>`JNG rel16off`<br>`JNG rel32off`|7E cb<br>0F 8E cw<br>0F 8E cd|Jump if not greater (`ZF` = 1 or `SF` <> `OF`)|
|`JNLE rel8off`<br>`JNLE rel16off`<br>`JNLE rel32off`|7F cb<br>0F 8F cw<br>0F 8F cd|Jump if not less or equal (`ZF` = 0 and `SF` = `OF`)|
|`JG rel8off`<br>`JG rel16off`<br>`JG rel32off`|7F cb<br>0F 8F cw<br>0F 8F cd|Jump if greater (`ZF` = 0 and `SF` = `OF`)|
|`JCXZ rel8off`<br>`JECXZ rel8off`<br>`JRCXZ rel8off`|E3 cb|Jump short if count register `CX`,`ECX`,`RCX` is zero|

# JMP指令表
|指令|Opcode|说明|
|-|-|-|
|JMP rel8off |EB cb| Short jump with the target specified by an 8-bit signeddisplacement.|
|JMP rel16off |E9 cw| Near jump with the target specified by a 16-bit signed displacement.|
|JMP rel32off |E9 cd| Near jump with the target specified by a 32-bit signed displacement.|
|JMP reg/mem16 |FF /4| Near jump with the target specified reg/mem16.|
|JMP reg/mem32 |FF /4| Near jump with the target specified reg/mem32.(No prefix for encoding in 64-bit mode.)|
|JMP reg/mem64 |FF /4| Near jump with the target specified reg/mem64.|
|JMP FAR pntr16:16 |EA cd| Far jump direct, with the target specified by a far pointer contained in the instruction. (Invalid in 64-bit mode.)|
|JMP FAR pntr16:32 |EA cp| Far jump direct, with the target specified by a far pointer contained in the instruction. (Invalid in 64-bit mode.)|
|JMP FAR mem16:16 |FF /5| Far jump indirect, with the target specified by a far pointer in memory (16-bit operand size).|
|JMP FAR mem16:32 |FF /5| Far jump indirect, with the target specified by a far pointer in memory (32- and 64-bit operand size).|

References: AMD64 Architecture Programmer’s Manual Volume 3: General-Purpose and System Instructions

|Reference|Label|
|-|-|
|REX|1.2.7 REX Prefix|
|ModRM|1.4.1 ModRM Byte Format|
|SIB|1.4.2 SIB Byte Format|

