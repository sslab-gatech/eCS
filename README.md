# eCS (Enlightened Critical Sections)

eCS (Enlightened Critical Sections) strive to address all preemption problems together by bridging the semantic gap between guest OSes and the hypervisor: the hypervisor now knows whether guest OSes are running in critical sections and a guest OS has hypervisor’s scheduling context.

We annotate all critical sections by using the lightweight para-virtualized APIs, so we called enlightened critical sections (eCS), that provide scheduling hints to both the hypervisor and VMs. The hypervisor uses the hint to reschedule a vCPU to fundamentally overcome the double scheduling problem for these annotated critical sections and VMs use the hypervisor provided hints to further mitigate the blocked-waiter wake-up problem.

This project is provided under the terms of the GNU General Public License v2.0 like the Linux kernel.

## Build and install the kernel

      - eCS was developed on Linux 4.13.
      - The Linux kernel source is available in ./linux/.
      - Select CONFIG_PARAVIRT_VCS option while compiling the Linex kernel.
      - Build and install the kernel.

## Reference
https://www.usenix.org/conference/atc18/presentation/kashyap
```
@inproceedings{kashyap2018scaling,
  title={Scaling Guest OS Critical Sections with eCS},
  author={Kashyap, Sanidhya and Min, Changwoo and Kim, Taesoo},
  booktitle={2018 USENIX Annual Technical Conference (USENIX ATC 18)},
  pages={159--172},
  year={2018}
}
```
