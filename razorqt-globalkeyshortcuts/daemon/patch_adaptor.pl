#!/usr/bin/perl

while (<>)
{
	chomp;
	if ($_ =~ m/^([ \t]*)::DBus::ReturnMessage reply\(call\);$/)
	{
		$output[$#output + 1] = $output[$#output];
		$output[$#output - 1] = $1.'mCall = &call;';
		$output[$#output + 1] = $1.'mCall = 0;';
		$output[$#output + 1] = $_;
	}
	elsif ($_ =~ m/^([ \t]*): ::DBus::InterfaceAdaptor/)
	{
		$indent = $1;
		$output[$#output + 1] = $_;
		$output[$#output + 1] = $indent.', mCall(0)';
	}
	elsif ($_ =~ m/^};$/)
	{
		$output[$#output + 1] = '';
		$output[$#output + 1] = 'protected:';
		$output[$#output + 1] = '';
		$output[$#output + 1] = $indent.'const ::DBus::CallMessage *mCall;';
		$output[$#output + 1] = $_;
	}
	else
	{
		$output[$#output + 1] = $_;
	}
}

print join("\n",@output);
