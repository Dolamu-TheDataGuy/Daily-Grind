# Mailroom Receivers — Value vs Pointer Receivers in Go

**Date:** 2026-08-18

**Language:** Go

**Source:** boot.dev

**Concepts:** value receivers · pointer receivers · mutability · nil-pointer safety · automatic referencing & dereferencing · method sets · struct copy cost

---

## 🎯 The Problem (one sentence)

Implement methods on a `Parcel` struct that demonstrate the difference between value receivers (which operate on a copy) and pointer receivers (which mutate the original), plus helper functions that call those methods through both values and pointers — safely handling `nil` pointers.

---

## 🧩 My First Approach

The challenge spelled out which receiver each method needed, so the design decisions were made for me. The real work was understanding *why* each choice was specified — and making sure my implementation actually produced the behaviour the spec described.

I worked through the six pieces in order:

- `PreviewStamp` — value receiver, adds a stamp to its local copy only.
- `AddStamp` — pointer receiver, adds a stamp to the real parcel, no-op when `nil`.
- `Send` — pointer receiver, sets `Sent = true` only when `Stamps > 0`, no-op when `nil`.
- `processByValue` — takes a `Parcel` value, runs all three methods, returns the updated local parcel while leaving the caller's parcel untouched.
- `processByPointer` — takes a `*Parcel`, runs the same three methods, mutating the real parcel in place.
- `safeStampCount` — returns `0` for a `nil` pointer, otherwise the stamp count.

---

## 💥 What Broke / Where I Got Stuck

**The mental hurdle: `PreviewStamp` looks like it does nothing.**

```go
func (p Parcel) PreviewStamp() {
	p.Stamps += 1
}
```

This method increments a field and then throws the result away. My instinct was that it must be a bug — why write a method that has no effect? But that *is* the lesson. With a value receiver, `p` is a fresh copy created at call time. The `+= 1` genuinely happens, on the copy, and the copy is discarded the moment the method returns. The caller's parcel never sees it.

**The bug I actually shipped: `processByPointer` panics on `nil`.**

I put nil guards inside `AddStamp` and `Send`, which was right. But `processByPointer` calls `PreviewStamp` *first*:

```go
func processByPointer(p *Parcel) {
	p.PreviewStamp()  // ← panics if p is nil
	p.AddStamp()
	p.Send()
}
```

`PreviewStamp` has a **value receiver**. To call a value-receiver method through a pointer, Go automatically dereferences it — `p.PreviewStamp()` becomes `(*p).PreviewStamp()`. Dereferencing a `nil` pointer panics before the method body ever runs. So a nil guard *inside* `PreviewStamp` wouldn't help either; the copy has to be made before the method can start.

I confirmed this by running it:

```
PANIC in processByPointer(nil): runtime error: invalid memory address or nil pointer dereference
```

The fix is a guard at the top of the helper, not inside the method:

```go
func processByPointer(p *Parcel) {
	if p == nil {
		return
	}
	p.PreviewStamp()
	p.AddStamp()
	p.Send()
}
```

**Why the tests still passed:** the test cases exercised `processByPointer` with a real parcel and exercised `nil` only through `safeStampCount`, `AddStamp`, and `Send` — all of which had their guards. The panic path was never triggered. A correct-looking solution that only fails on an untested input is exactly the kind of bug worth catching in a write-up.

---

## 💡 The Eureka Moment

**Value receiver = a photocopy. Pointer receiver = the original document.**

When you call a value-receiver method, Go hands the method a photocopy of the struct. Scribble on it all you like — the original in the filing cabinet is untouched, and the photocopy goes in the bin when the method ends. When you call a pointer-receiver method, Go hands over the *address* of the original. Any change is made to the document itself.

That single distinction explains everything the challenge tests:

```go
original := Parcel{Label: "Maps", Stamps: 1, Sent: false}
updated := processByValue(original)

fmt.Println(original)  // {Maps 1 false}  ← untouched, it was copied in
fmt.Println(updated)   // {Maps 2 true}   ← the copy, fully processed
```

The `original` survives because `processByValue` received a copy. Inside the function, that copy gets mutated by the pointer-receiver methods and returned. Two different parcels, two different outcomes, from the same starting value.

**The second click: Go silently converts between values and pointers for you.**

Inside `processByValue`, `p` is a `Parcel` value — not a pointer. Yet this compiles and works:

```go
func processByValue(p Parcel) Parcel {
	p.PreviewStamp()  // value receiver on a value — direct
	p.AddStamp()      // pointer receiver on a VALUE — Go rewrites this as (&p).AddStamp()
	p.Send()          // same
	...
}
```

Go automatically takes the address of `p` because `p` is an *addressable* local variable. Likewise, calling a value-receiver method through a pointer auto-dereferences. This convenience is why the code reads so cleanly — and also why the nil panic above is so easy to miss, since the dereference is invisible in the source.

---

## ✅ How the Final Solution Works

```go
package main

type Parcel struct {
	Label  string
	Stamps int
	Sent   bool
}

// VALUE receiver — operates on a copy, caller's parcel unchanged.
func (p Parcel) PreviewStamp() {
	p.Stamps += 1
}

// POINTER receiver — mutates the real parcel.
func (p *Parcel) AddStamp() {
	if p == nil {
		return
	}
	p.Stamps += 1
}

// POINTER receiver — mutates the real parcel, conditionally.
func (p *Parcel) Send() {
	if p == nil {
		return
	}
	if p.Stamps > 0 {
		p.Sent = true
	}
}

func processByValue(p Parcel) Parcel {
	p.PreviewStamp()
	p.AddStamp()
	p.Send()
	return Parcel{
		Label:  p.Label,
		Stamps: p.Stamps,
		Sent:   p.Sent,
	}
}

func processByPointer(p *Parcel) {
	p.PreviewStamp()
	p.AddStamp()
	p.Send()
}

func safeStampCount(p *Parcel) int {
	if p == nil {
		return 0
	}
	return p.Stamps
}
```

**Verified output** — matches the spec exactly:

| Call | Result |
|---|---|
| `original` after `processByValue(original)` | `{Maps 1 false}` — unchanged ✓ |
| `updated := processByValue(original)` | `{Maps 2 true}` ✓ |
| `parcel` after `processByPointer(&parcel)` | `{Seeds 2 true}` ✓ |
| `safeStampCount(nil)` | `0` ✓ |

Step by step through the decisions:

1. **`PreviewStamp` takes a value receiver deliberately.** It's a *preview* — it shows what would happen without committing. The discarded copy is the feature, not a bug.

2. **`AddStamp` and `Send` take pointer receivers** because they must persist their changes. Without the pointer, the caller would never see the stamp added or the parcel marked as sent.

3. **Nil guards live inside the pointer-receiver methods.** Go permits calling a pointer-receiver method on a `nil` pointer — the method runs with `p == nil`, and no panic occurs *unless* the body dereferences `p`. That's why `if p == nil { return }` at the top works: it exits before touching any field.

4. **`processByValue` receives a copy, so the caller is automatically protected.** The parameter itself is the isolation mechanism — no defensive copying needed inside the function.

5. **The explicit struct literal in the return is redundant.** `return p` produces the identical result, since `p` is already the fully-processed local copy. Rebuilding it field-by-field works, but adds a maintenance cost: add a fourth field to `Parcel` later and this return statement silently drops it. Worth simplifying:

```go
func processByValue(p Parcel) Parcel {
	p.PreviewStamp()
	p.AddStamp()
	p.Send()
	return p          // same result, survives future field additions
}
```

6. **`processByPointer` needs a nil guard at the top** — see the bug section above. The guards inside `AddStamp` and `Send` don't protect the `PreviewStamp` call that precedes them.

---

## 📚 Lessons Learnt

**1. Value receivers copy; pointer receivers mutate.**
With a value receiver, Go passes a copy of the struct and the original is preserved. With a pointer receiver, Go passes the memory address, so the method modifies the original in place. This is the single distinction that drives every other decision about receivers.

**2. Choose the receiver by intent — preserve or persist.**
Use a **value receiver** when the method should not affect the caller's data: read-only accessors, calculations, previews, and anything returning a modified copy. Use a **pointer receiver** when the change must persist beyond the method call: setters, mutators, state transitions, and anything that updates the struct.

**3. Struct size matters as much as mutability.**
Beyond correctness, receiver choice affects performance:

- **Large structs** — copying a struct with many fields (or large arrays) on every method call costs memory and CPU. A pointer receiver passes only an 8-byte address regardless of struct size, so it's the right choice even for methods that don't mutate anything.
- **Small / primitive structs** — types like `Point{X, Y int}` or standard library types such as `time.Time` are cheap to copy and are conventionally passed by value.

**4. Keep receiver types consistent across a type's method set.**
Go convention strongly favours uniformity: if *any* method on a struct needs a pointer receiver — whether for mutation or for performance — make *all* methods on that struct use pointer receivers. Mixing them (as this challenge deliberately does, for teaching purposes) creates subtle bugs and causes confusion about which method set satisfies an interface. Only a `*T` has access to both value and pointer methods; a plain `T` only has the value-receiver methods.

**5. Go automatically converts between values and pointers — which can hide a panic.**
`value.PointerMethod()` becomes `(&value).PointerMethod()`, and `pointer.ValueMethod()` becomes `(*pointer).ValueMethod()`. The second form is the dangerous one: calling a **value-receiver method on a nil pointer panics**, because Go must dereference to build the copy before the method body runs. A nil check inside the method cannot save you — the guard has to be at the call site.

**6. Nil pointer receivers are safe until you dereference.**
`nilPointer.PointerMethod()` runs fine and enters the method body with `p == nil`. It only panics when the body accesses a field. This is why the `if p == nil { return }` idiom at the top of a pointer-receiver method is genuinely useful, and why it is *not* available for value-receiver methods.

---

## 🔗 Further Reading

- [A Tour of Go — Pointer receivers](https://go.dev/tour/methods/4)
- [A Tour of Go — Choosing a value or pointer receiver](https://go.dev/tour/methods/8)
- [Go Wiki — Method sets and receiver types](https://go.dev/wiki/CodeReviewComments#receiver-type)
- Next to explore: how receiver choice affects **interface satisfaction** — why a `Parcel` value fails to satisfy an interface whose methods have pointer receivers, while `*Parcel` succeeds.