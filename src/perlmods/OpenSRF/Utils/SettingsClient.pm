use strict; use warnings;
package OpenSRF::Utils::SettingsClient;
use OpenSRF::Utils::SettingsParser;
use OpenSRF::System;
use OpenSRF::AppSession;
use OpenSRF::Utils::Config;
use OpenSRF::EX qw(:try);

use vars qw/$host_config/;


sub new {return bless({},shift());}
my $session;
$host_config = undef;

my $we_cache = 1;
sub set_cache {
	my($self, $val) = @_;
	if(defined($val)) { $we_cache = $val; }
}



# ------------------------------------
# utility method for grabbing config info
sub config_value {
	my($self,@keys) = @_;


	my $bsconfig = OpenSRF::Utils::Config->current;
	my $host = $bsconfig->env->hostname;

	if($we_cache) {
		if(!$host_config) { grab_host_config($host); }
	} else {
		grab_host_config($host);
	}

	if(!$host_config) {
		throw OpenSRF::EX::Config ("Unable to retrieve host config for $host" );
	}

	my $hash = $host_config;

	# XXX TO DO, check local config 'version', 
	# call out to settings server when necessary....
	try {
		for my $key (@keys) {
			$hash = $hash->{$key};
		}

	} catch Error with {
		my $e = shift;
		throw OpenSRF::EX::Config ("No Config information for @keys : $e : $@");
	};

	return $hash;

}


# XXX make smarter and more robust...
sub grab_host_config {

	my $host = shift;

	warn "Grabbing Host config for $host\n";
	OpenSRF::System->bootstrap_client(client_name => "system_client");
	$session = OpenSRF::AppSession->create( "opensrf.settings" ) unless $session;
	my $bsconfig = OpenSRF::Utils::Config->current;

	my $resp;
	my $req;
	try {

		if( ! ($session->connect()) ) {die "Settings Connect timed out\n";}
		$req = $session->request( "opensrf.settings.host_config.get", $host );
		$resp = $req->recv( timeout => 10 );

	} catch OpenSRF::EX with {

		my $e = shift;
		warn "Connection to Settings Failed  $e : $@ ***\n";
		die $e;
	};

	if(!$resp) {
		warn "No Response from settings server...going to sleep\n";
		sleep;
	}

	if( $resp && UNIVERSAL::isa( $resp, "OpenSRF::EX" ) ) {
		throw $resp;
	}

	$host_config = $resp->content();
	$req->finish();
	$session->finish;
	$session->disconnect();
}



1;