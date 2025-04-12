using FastEndpoints;

namespace ServerDC;

public sealed record SampleRequest();

public sealed record SampleResponse(string Status);

internal sealed class SampleValidator : Validator<SampleRequest>
{
    public SampleValidator()
    {
        //RuleFor(r => r.Property).NotEmpty();
    }
}

public sealed class SampleEndpoint : Endpoint<SampleRequest, SampleResponse>
{
    public override void Configure()
    {
        Get("/sample");
        AllowAnonymous();
    }

    public override Task HandleAsync(SampleRequest request, CancellationToken ct)
    {
        return SendOkAsync(new SampleResponse("ok"));
    }
}
