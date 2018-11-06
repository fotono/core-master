# FCP (fotono Consensus Protocol)

The FCP subsystem is an abstract implementation of FCP, a protocol for federated
byzantine agreement, intended to drive a distributed system built around the
"replicated state machine" formalism. FCP is defined without reference to any
particular interpretation of the concepts of "slot" or "value", nor any
particular network communication system or replicated state machine.

This separation from the rest of the system is intended to make the
implementation of FCP easier to model, compare to the paper describing the
protocol, audit for correctness, and extract for reuse in different programs at
a later date.

The central [FCP class](FCP.h) should be subclassed by any module wishing to
implement consensus using the FCP protocol, implementing the necessary abstract
methods for handling FCP-generated events, and calling FCP base-class methods to
receive incoming messages. The messages making up the protocol are defined in
XDR, in the file [fotono-FCP.x](../xdr/fotono-FCP.x)

The `fotono-core` program has a single subclass of FCP called
[Herder](../herder), which gives a specific interpretation to "slot" and
"value", and connects FCP up with a specific broadcast communication medium
([Overlay](../overlay)) and specific replicated state machine
([LedgerManager](../ledger)).

For details of the protocol itself, see the [paper on FCP](https://www.fotono.org/papers/fotono-consensus-protocol.pdf).
