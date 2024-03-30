# Serviço de Notícias

---
title: Trabalho Prático 1 - Meta 1
subtitle: Redes de Comunicação
author: Bernardo Haab, Luís Góis
date: date
---

## Descrição do ambiente GNS3

Os seguintes comandos foram utilizados para configurar o projeto GNS3. Foi
também criada uma imagens docker nomeada `project-server`, a partir do arquivo
Dockerfile que se encontra no diretório `servidor`.

### Server

```resolv.conf
# Static config for eth0
auto eth0
iface eth0 inet static
address 193.137.100.1
netmask 255.255.255.128
gateway 193.137.100.126
up echo nameserver 193.137.100.1 > /etc/resolv.conf
```

### Cliente 1

```resolv.conf
# Static config for eth0
auto eth0
iface eth0 inet static
address 10.5.2.1
netmask 255.255.255.192
gateway 10.5.2.62
```

### Cliente 2

```resolv.conf
# Static config for eth0
auto eth0
iface eth0 inet static
address 10.5.2.2
netmask 255.255.255.192
gateway 10.5.2.62
```

### Cliente 3

```resolv.conf
# Static config for eth0
auto eth0
iface eth0 inet static
address 193.137.100.129
netmask 255.255.255.128
gateway 193.137.100.254
```

### R1

```tcl
configure terminal

interface f0/0
ip address 193.137.100.126 255.255.255.128
no shutdown
exit

interface f0/1
ip address 193.137.101.254 255.255.255.0
no shutdown
exit

ip route 193.137.100.128 255.255.255.128 193.137.101.253
exit
```

### R2

```tcl
configure terminal

interface f0/0
ip address 193.137.101.253 255.255.255.0
no shutdown
exit

interface f0/1
ip address 193.137.100.254 255.255.255.128
no shutdown
exit

ip route 193.137.100.0 255.255.255.128 193.137.101.254
exit
```

### R3

```tcl
configure terminal

access-list 30 permit 10.5.2.0 0.0.0.63
ip nat inside source list 30 interface f0/0 overload

interface f0/0
ip address 193.137.101.252 255.255.255.0
ip nat outside
no shutdown
exit

ip address 10.5.2.62 255.255.255.192
ip nat inside
no shutdown
exit
```
