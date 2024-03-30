# Connectivity

![Topology](/assets/topology.png)

## Methodology

```sh
ping -c 3 <ip address>
```

## Results

From             | To              | Connectivity
---              | ---             | ---
 10.5.2.1        | 193.137.100.1   | <input type="checkbox" disabled checked />
 10.5.2.2        | 193.137.100.1   | <input type="checkbox" disabled checked />
 193.137.100.129 | 193.137.100.1   | <input type="checkbox" disabled />
 193.137.100.1   | 193.137.100.129 | <input type="checkbox" disabled />
 193.137.100.1   | 10.5.2.1        | <input type="checkbox" disabled />
 193.137.100.1   | 10.5.2.2        | <input type="checkbox" disabled />
