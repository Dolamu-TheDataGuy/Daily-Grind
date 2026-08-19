package solution

type Parcel struct {
	Label  string
	Stamps int
	Sent   bool
}

func (p Parcel) PreviewStamp() {
	p.Stamps += 1
}

func (p *Parcel) AddStamp() {
	if p == nil {
		return
	}
	p.Stamps += 1
}

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
		Label: p.Label,
		Stamps: p.Stamps,
		Sent: p.Sent,
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
