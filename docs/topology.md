# Topology

![Topology before calculating networks and
ips](/assets/topology-undefined.png)

At the IPv4 addressing level, the following requirements must be taken into
account when configuring the communications scenario:

1. The network to which Client1 and Client2 belong (Network D) uses private
  addresses, and router R3 implements NAT. Use the **10.5.2.0/26** network to
  address all interfaces on Network D.

## Network D

$10.5.2.0_{10} = 0000 1010.0000 0101.0000 0010.0000 0000_{2}$

### Network D - Subnet Mask

$/26 = 11111111.11111111.11111111.11000000_{2} = 255.255.255.192_{10}$

### Network D - Wildcard Mask

$00000000.00000000.00000000.00111111_{2} = 0.0.0.63_{10}$

### Network D - Lowest ip address on this network

$0000 1010.0000 0101.0000 0010.0000 0001_{2} = 10.5.2.1_{10}$

### Network D - Highest ip address (excluding broadcast)

$0000 1010.0000 0101.0000 0010.0011 1110_{2} = 10.5.2.62_{10}$

### Network D - Broadcast ip address

$0000 1010.0000 0101.0000 0010.0011 1111_{2} = 10.5.2.63_{10}$

Network D:

|                                       |                      |
| ---                                   | ---                  |
| Ip Address                            | 10.5.2.0             |
| Subnet Mask (Dotted decimal notation) | 255.255.255.192      |
| Subnet Mask (CIDR notation)           | /26                  |
| Wildcard Mask                         | 0.0.0.63             |
| Usable Host IP Range                  | 10.5.2.1 - 10.5.2.62 |
| Broadcast Address                     | 10.5.2.63            |

2. Use the network **193.137.100.0/23** to address networks A, B and C. When
   dividing, ensure that `Network C` has **more** available addresses than the
   rest and that `Network A` has the **lowest addresses in the range**. In this
   division you should not waste addresses.

## Subnet

To achieve this, we will have to divide the network `193.137.100.0/23` using
subnetting.
We can start by dividing the network in 2 networks and pick one with the highest
addresses to divide again.

```
/23
  ├── 1. /24
  │        ├── 3. /25 (A)
  │        └── 4. /25 (B)
  └── 2. /24 (C)
```

$193.137.100.0_{10} = 1100 0001.1000 1001.0110 0100.0000 0000_{2}$

$/23 = 11111111.11111111.11111110.00000000_{2} = 255.255.254.0_{10}$

Now we will subnet the network `193.137.100.0/23` into 2 networks. We will place
in the 24th bit 0 for network 1 (in the tree graph) and in the 24th bit 1 for
network 2.

Network 1:
$1100 0001.1000 1001.0110 0100.0000 0000_{2} = 193.137.100.0_{10}$

Network 2:
$1100 0001.1000 1001.0110 0101.0000 0000_{2} = 193.137.101.0_{10}$

### Network C

Network 2 `193.137.101.0/24` will be assigned to Network C in our topology.

#### Network C - Subnet Mask

$/24 = 11111111.11111111.11111111.00000000_{2} = 255.255.255.0_{10}$

#### Network C - Wildcard Mask

$00000000.00000000.00000000.11111111_{2} = 000.000.000.255_{10}$

#### Network C - Lowest IP Address

$1100 0001.1000 1001.0110 0101.0000 0001_{2} = 193.137.101.1_{10}$

#### Network C - Highest IP Address

$1100 0001.1000 1001.0110 0101.1111 1110_{2} = 193.137.101.254_{10}$

#### Network C - Broadcast

$1100 0001.1000 1001.0110 0101.1111 1111_{2} = 193.137.101.255_{10}$

Network C:

|                                       |                                 |
| ---                                   | ---                             |
| Ip Address                            | 193.137.101.0                   |
| Subnet Mask (Dotted decimal notation) | 255.255.255.0                   |
| Subnet Mask (CIDR notation)           | /24                             |
| Wildcard Mask                         | 0.0.0.255                       |
| Usable Host IP Range                  | 193.137.101.1 - 193.137.101.254 |
| Broadcast Address                     | 193.137.101.255                 |

Now we will pick network 1 `193.137.100.0/24` and subnet it into more 2 networks
`/25` and place in the 25th bit 0 for network 3 (having the lowest ip addresses
in all range of `193.137.100.0/23`) and in the 25th bit 1 for network 4.

Network 3 will be assigned to A and network 4 assigned to B.

### Network A

$1100 0001.1000 1001.0110 0100.0000 0000_{2} = 193.137.100.0_{10}$

#### Network A - Subnet Mask

$/25 = 11111111.11111111.11111111.10000000_{2} = 255.255.255.128_{10}$

#### Network A - Wildcard Mask

$00000000.00000000.00000000.01111111_{2} = 0.0.0.127_{10}$

#### Network A - Lowest IP Address

$1100 0001.1000 1001.0110 0100.0000 0001_{2} = 193.137.100.1_{10}$

#### Network A - Highest IP Address

$1100 0001.1000 1001.0110 0100.011111110_{2} = 193.137.100.126_{10}$

#### Network A - Broadcast

$1100 0001.1000 1001.0110 0100.011111111_{2} = 193.137.100.127_{10}$

Network A:

|                                       |                                 |
| ---                                   | ---                             |
| Ip Address                            | 193.137.100.0                   |
| Subnet Mask (Dotted decimal notation) | 255.255.255.128                 |
| Subnet Mask (CIDR notation)           | /25                             |
| Wildcard Mask                         | 0.0.0.127                       |
| Usable Host IP Range                  | 193.137.100.1 - 193.137.100.126 |
| Broadcast Address                     | 193.137.100.127                 |

### Network B

$1100 0001.1000 1001.0110 0100.1000 0000_{2} = 193.137.100.128_{10}$

#### Network B - Subnet Mask

$/25 = 11111111.11111111.11111111.10000000_{2} = 255.255.255.128_{10}$

#### Network B - Wildcard Mask

$00000000.00000000.00000000.01111111_{2} = 0.0.0.127_{10}$

#### Network B - Lowest IP Address

$1100 0001.1000 1001.0110 0100.1000 0001_{2} = 193.137.100.129_{10}$

#### Network B - Highest IP Address

$1100 0001.1000 1001.0110 0100.111111110_{2} = 193.137.100.254_{10}$

#### Network B - Broadcast

$1100 0001.1000 1001.0110 0100.111111111_{2} = 193.137.100.255_{10}$

Network B:

|                                       |                                   |
| ---                                   | ---                               |
| Ip Address                            | 193.137.100.128                   |
| Subnet Mask (Dotted decimal notation) | 255.255.255.128                   |
| Subnet Mask (CIDR notation)           | /25                               |
| Wildcard Mask                         | 0.0.0.127                         |
| Usable Host IP Range                  | 193.137.100.129 - 193.137.100.254 |
| Broadcast Address                     | 193.137.100.255                   |

3. All equipment must be assigned appropriate IP addresses, within the agreed
  subnet range.

With all the network computed, we can now assign the lowest ip addresses of a
given network for the terminal devices interfaces and highest ip addresses of a
network for the routers' interfaces

Network A:

|                                       |                                 |
| ---                                   | ---                             |
| Ip Address                            | 193.137.100.0                   |
| Subnet Mask (Dotted decimal notation) | 255.255.255.128                 |
| Subnet Mask (CIDR notation)           | /25                             |
| Wildcard Mask                         | 0.0.0.127                       |
| Usable Host IP Range                  | 193.137.100.1 - 193.137.100.126 |
| Broadcast Address                     | 193.137.100.127                 |

Network B:

|                                       |                                   |
| ---                                   | ---                               |
| Ip Address                            | 193.137.100.128                   |
| Subnet Mask (Dotted decimal notation) | 255.255.255.128                   |
| Subnet Mask (CIDR notation)           | /25                               |
| Wildcard Mask                         | 0.0.0.127                         |
| Usable Host IP Range                  | 193.137.100.129 - 193.137.100.254 |
| Broadcast Address                     | 193.137.100.255                   |

Network C:

|                                       |                                 |
| ---                                   | ---                             |
| Ip Address                            | 193.137.101.0                   |
| Subnet Mask (Dotted decimal notation) | 255.255.255.0                   |
| Subnet Mask (CIDR notation)           | /24                             |
| Wildcard Mask                         | 0.0.0.255                       |
| Usable Host IP Range                  | 193.137.101.1 - 193.137.101.254 |
| Broadcast Address                     | 193.137.101.255                 |

Network D:

|                                       |                      |
| ---                                   | ---                  |
| Ip Address                            | 10.5.2.0             |
| Subnet Mask (Dotted decimal notation) | 255.255.255.192      |
| Subnet Mask (CIDR notation)           | /26                  |
| Wildcard Mask                         | 0.0.0.63             |
| Usable Host IP Range                  | 10.5.2.1 - 10.5.2.62 |
| Broadcast Address                     | 10.5.2.63            |

In addition to communication, router R3 must support SNAT (Source NAT), where
the network of Client1 and Client2 is the internal network.

![Topology](/assets/topology.png)
