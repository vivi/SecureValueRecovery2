// Copyright 2023 Signal Messenger, LLC
// SPDX-License-Identifier: AGPL-3.0-only

#include "attestation/tpm2snp/tpm2snp.h"
#include "attestation/sev/sev.h"
#include "util/macros.h"
#include "util/log.h"
#include "hmac/hmac.h"
#include "util/constant.h"
#include "util/hex.h"
#include "util/base64.h"
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/base.h>
#include <rapidjson/document.h>

namespace svr2::attestation::tpm2snp {
namespace {

const char* MSFT_AKCERT_ROOT = R"EOF(
-----BEGIN CERTIFICATE-----
MIIFsDCCA5igAwIBAgIQUfQx2iySCIpOKeDZKd5KpzANBgkqhkiG9w0BAQwFADBp
MQswCQYDVQQGEwJVUzEeMBwGA1UEChMVTWljcm9zb2Z0IENvcnBvcmF0aW9uMTow
OAYDVQQDEzFBenVyZSBWaXJ0dWFsIFRQTSBSb290IENlcnRpZmljYXRlIEF1dGhv
cml0eSAyMDIzMB4XDTIzMDYwMTE4MDg1M1oXDTQ4MDYwMTE4MTU0MVowaTELMAkG
A1UEBhMCVVMxHjAcBgNVBAoTFU1pY3Jvc29mdCBDb3Jwb3JhdGlvbjE6MDgGA1UE
AxMxQXp1cmUgVmlydHVhbCBUUE0gUm9vdCBDZXJ0aWZpY2F0ZSBBdXRob3JpdHkg
MjAyMzCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBALoMMwvdRJ7+bW00
adKE1VemNqJS+268Ure8QcfZXVOsVO22+PL9WRoPnWo0r5dVoomYGbobh4HC72s9
sGY6BGRe+Ui2LMwuWnirBtOjaJ34r1ZieNMcVNJT/dXW5HN/HLlm/gSKlWzqCEx6
gFFAQTvyYl/5jYI4Oe05zJ7ojgjK/6ZHXpFysXnyUITJ9qgjn546IJh/G5OMC3mD
fFU7A/GAi+LYaOHSzXj69Lk1vCftNq9DcQHtB7otO0VxFkRLaULcfu/AYHM7FC/S
q6cJb9Au8K/IUhw/5lJSXZawLJwHpcEYzETm2blad0VHsACaLNucZL5wBi8GEusQ
9Wo8W1p1rUCMp89pufxa3Ar9sYZvWeJlvKggWcQVUlhvvIZEnT+fteEvwTdoajl5
qSvZbDPGCPjb91rSznoiLq8XqgQBBFjnEiTL+ViaZmyZPYUsBvBY3lKXB1l2hgga
hfBIag4j0wcgqlL82SL7pAdGjq0Fou6SKgHnkkrV5CNxUBBVMNCwUoj5mvEjd5mF
7XPgfM98qNABb2Aqtfl+VuCkU/G1XvFoTqS9AkwbLTGFMS9+jCEU2rw6wnKuGv1T
x9iuSdNvsXt8stx4fkVeJvnFpJeAIwBZVgKRSTa3w3099k0mW8qGiMnwCI5SfdZ2
SJyD4uEmszsnieE6wAWd1tLLg1jvAgMBAAGjVDBSMA4GA1UdDwEB/wQEAwIBhjAP
BgNVHRMBAf8EBTADAQH/MB0GA1UdDgQWBBRL/iZalMH2M8ODSCbd8+WwZLKqlTAQ
BgkrBgEEAYI3FQEEAwIBADANBgkqhkiG9w0BAQwFAAOCAgEALgNAyg8I0ANNO/8I
2BhpTOsbywN2YSmShAmig5h4sCtaJSM1dRXwA+keY6PCXQEt/PRAQAiHNcOF5zbu
OU1Bw/Z5Z7k9okt04eu8CsS2Bpc+POg9js6lBtmigM5LWJCH1goMD0kJYpzkaCzx
1TdD3yjo0xSxgGhabk5Iu1soD3OxhUyIFcxaluhwkiVINt3Jhy7G7VJTlEwkk21A
oOrQxUsJH0f2GXjYShS1r9qLPzLf7ykcOm62jHGmLZVZujBzLIdNk1bljP9VuGW+
cISBwzkNeEMMFufcL2xh6s/oiUnXicFWvG7E6ioPnayYXrHy3Rh68XLnhfpzeCzv
bz/I4yMV38qGo/cAY2OJpXUuuD/ZbI5rT+lRBEkDW1kxHP8cpwkRwGopV8+gX2KS
UucIIN4l8/rrNDEX8T0b5U+BUqiO7Z5YnxCya/H0ZIwmQnTlLRTU2fW+OGG+xyIr
jMi/0l6/yWPUkIAkNtvS/yO7USRVLPbtGVk3Qre6HcqacCXzEjINcJhGEVg83Y8n
M+Y+a9J0lUnHytMSFZE85h88OseRS2QwqjozUo2j1DowmhSSUv9Na5Ae22ycciBk
EZSq8a4rSlwqthaELNpeoTLUk6iVoUkK/iLvaMvrkdj9yJY1O/gvlfN2aiNTST/2
bd+PA4RBToG9rXn6vNkUWdbLibU=
-----END CERTIFICATE-----
)EOF";

bssl::UniquePtr<STACK_OF(X509)> AzureRootsOfTrust() {
  bssl::UniquePtr<STACK_OF(X509)> root_stack(sk_X509_new_null());
  CHECK(root_stack.get() != nullptr);
  for (auto root : {MSFT_AKCERT_ROOT}) {
    bssl::UniquePtr<BIO> root_bio(BIO_new_mem_buf(root, -1));
    CHECK(root_bio.get() != nullptr);
    bssl::UniquePtr<X509> root_x509(PEM_read_bio_X509(root_bio.get(), nullptr, nullptr, nullptr));
    CHECK(root_x509.get() != nullptr);
    CHECK(0 != sk_X509_push(root_stack.get(), root_x509.get()));
    root_x509.release();  // owned by root_stack
  }
  return root_stack;
}

const char* GOOG_AKCERT_ROOT = R"EOF(
-----BEGIN CERTIFICATE-----
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
av16U4E=
-----END CERTIFICATE-----
)EOF";

bssl::UniquePtr<STACK_OF(X509)> GcpRootsOfTrust() {
  bssl::UniquePtr<STACK_OF(X509)> root_stack(sk_X509_new_null());
  CHECK(root_stack.get() != nullptr);
  for (auto root : {GOOG_AKCERT_ROOT}) {
    bssl::UniquePtr<BIO> root_bio(BIO_new_mem_buf(root, -1));
    CHECK(root_bio.get() != nullptr);
    bssl::UniquePtr<X509> root_x509(PEM_read_bio_X509(root_bio.get(), nullptr, nullptr, nullptr));
    CHECK(root_x509.get() != nullptr);
    CHECK(0 != sk_X509_push(root_stack.get(), root_x509.get()));
    root_x509.release();  // owned by root_stack
  }
  return root_stack;
}

const bssl::UniquePtr<STACK_OF(X509)> azure_roots_of_trust_smrtptr = AzureRootsOfTrust();
const bssl::UniquePtr<STACK_OF(X509)> gcp_roots_of_trust_smrtprt = GcpRootsOfTrust();

}  // namespace

const STACK_OF(X509)* const azure_roots_of_trust = azure_roots_of_trust_smrtptr.get();
const STACK_OF(X509)* const gcp_roots_of_trust = gcp_roots_of_trust_smrtprt.get();

std::pair<std::string, error::Error> SNPReportBufferFromAzureBuffer(const std::string& tpm2_buffer) {
  if (tpm2_buffer.size() < 1236) {
    return std::make_pair("", COUNTED_ERROR(AzureSNP_AzureBufferTooSmall));
  }
  return std::make_pair(tpm2_buffer.substr(32, 1184), error::OK);
}

std::pair<std::string, error::Error> RuntimeDataBufferFromAzureBuffer(const std::string& tpm2_buffer) {
  if (tpm2_buffer.size() < 1236) {
    return std::make_pair("", COUNTED_ERROR(AzureSNP_AzureBufferTooSmall));
  }
  // The runtime data buffer may be suffixed with any number of \0 bytes.
  // These bytes are ignored for the purpose of hashing, etc, so we strip
  // them off with strnlen.
  const char* start = tpm2_buffer.data() + 1236;
  size_t size = strnlen(start, tpm2_buffer.size() - 1236);
  return std::make_pair(std::string(start, size), error::OK);
}

error::Error VerifyAKCert(context::Context* ctx, const ASNPEvidence& evidence, const ASNPEndorsements& endorsements, util::UnixSecs now, const STACK_OF(X509)* const roots_of_trust) {
  LOG(DEBUG) << "Parsing AKCert DER";
  auto akcert_start = reinterpret_cast<const uint8_t*>(evidence.akcert_der().data());
  bssl::UniquePtr<X509> akcert(d2i_X509(nullptr, &akcert_start, evidence.akcert_der().size()));
  if (!akcert) {
    return COUNTED_ERROR(AzureSNP_InvalidAKCert);
  }

  LOG(DEBUG) << "Parsing AKCert intermediate DER";
  auto intermediate_start = reinterpret_cast<const uint8_t*>(endorsements.intermediate_der().data());
  bssl::UniquePtr<X509> intermediate(d2i_X509(nullptr, &intermediate_start, endorsements.intermediate_der().size()));
  if (!intermediate) {
    return COUNTED_ERROR(AzureSNP_InvalidAKCertIntermediate);
  }

  LOG(DEBUG) << "Building X509 context to verify AKCert";
  bssl::UniquePtr<X509_STORE_CTX> x509ctx(X509_STORE_CTX_new());
  bssl::UniquePtr<X509_STORE> store(X509_STORE_new());
  bssl::UniquePtr<STACK_OF(X509)> intermediates(sk_X509_new_null());
  if (!x509ctx || !store || !intermediates ||
      0 == sk_X509_push(intermediates.get(), intermediate.get())) {
    return COUNTED_ERROR(AzureSNP_CryptoAllocate);
  }
  intermediate.release();  // now owned by [intermediates]
  if (!X509_STORE_CTX_init(x509ctx.get(), store.get(), akcert.get(), intermediates.get())) {
    return COUNTED_ERROR(AzureSNP_CryptoStoreInit);
  }

  LOG(DEBUG) << "Verifying AKCert against root-of-trust";
  X509_STORE_CTX_set0_trusted_stack(x509ctx.get(), const_cast<STACK_OF(X509)*>(roots_of_trust));
  X509_VERIFY_PARAM *param = X509_STORE_CTX_get0_param(x509ctx.get());
  X509_VERIFY_PARAM_set_time_posix(param, now);
  if (1 != X509_verify_cert(x509ctx.get())) {
    auto err = X509_STORE_CTX_get_error(x509ctx.get());
    LOG(ERROR) << "SEV attestation verify_cert err=" << err << ": " << X509_verify_cert_error_string(err);
    return COUNTED_ERROR(AzureSNP_AKCertificateChainVerify);
  }

  LOG(DEBUG) << "Verifying that SNP report is valid";
  auto sevsnp_endorsements = ctx->Protobuf<attestation::sev::SevSnpEndorsements>();
  sevsnp_endorsements->set_vcek_der(endorsements.vcek_der());
  sevsnp_endorsements->set_ask_der(endorsements.ask_der());
  auto [snp_report, snperr] = ReportFromVerifiedBuffer(evidence.snp_report(), *sevsnp_endorsements, now);
  RETURN_IF_ERROR(snperr);
  LOG(INFO) << snp_report;

  LOG(DEBUG) << "Verifying that runtime data is verified by SNP report";
  auto runtimedata_sha256 = hmac::Sha256(evidence.runtime_data());
  LOG(DEBUG) << "Runtime data: " << evidence.runtime_data();
  if (!util::ConstantTimeEqualsBytes(runtimedata_sha256.data(), snp_report.report_data, runtimedata_sha256.size())) {
    return COUNTED_ERROR(AzureSNP_ReportDataMismatch);
  }

  LOG(DEBUG) << "Parsing runtime JSON to pull out and verify HCLAkPub";
  rapidjson::Document runtime_doc;
  if (runtime_doc.Parse(evidence.runtime_data().c_str()).HasParseError() ||
      !runtime_doc.HasMember("keys") ||
      !runtime_doc["keys"].IsArray() ||
      runtime_doc["keys"].Size() < 1) {
    return COUNTED_ERROR(AzureSNP_RuntimeDataJSON);
  }
  const auto& runtime_keys = runtime_doc["keys"];
  size_t key_idx = 0;
  for (; key_idx < runtime_keys.Size(); key_idx++) {
    const auto& runtime_key = runtime_keys[key_idx];
    if (runtime_key.HasMember("kid") &&
        runtime_key["kid"].IsString() &&
        0 == strcmp(runtime_key["kid"].GetString(), "HCLAkPub") &&
        runtime_key.HasMember("kty") &&
        runtime_key["kty"].IsString() &&
        0 == strcmp(runtime_key["kty"].GetString(), "RSA")) {
      break;
    }
  }
  if (key_idx >= runtime_keys.Size()) {
    return COUNTED_ERROR(AzureSNP_RuntimeDataJSON);
  }
  const auto& runtime_key = runtime_keys[key_idx];
  if (!runtime_key.HasMember("e") ||
      !runtime_key["e"].IsString() ||
      !runtime_key.HasMember("n") ||
      !runtime_key["n"].IsString()) {
    return COUNTED_ERROR(AzureSNP_RuntimeDataJSON);
  }
  std::string rte(runtime_key["e"].GetString());
  std::string rtn(runtime_key["n"].GetString());
  RETURN_IF_ERROR(util::B64DecodeInline(&rte, util::B64URL));
  RETURN_IF_ERROR(util::B64DecodeInline(&rtn, util::B64URL));

  LOG(DEBUG) << "Making sure HCLAkPub key matches public key certified by AKCert";
  bssl::UniquePtr<EVP_PKEY> akcert_pk(X509_get_pubkey(akcert.get()));
  if (!akcert_pk) { return COUNTED_ERROR(AzureSNP_AKCertPubKey); }
  auto* akcert_rsa = EVP_PKEY_get0_RSA(akcert_pk.get());  // non-owning reference
  if (!akcert_rsa) { return COUNTED_ERROR(AzureSNP_AKCertPubKey); }
  auto* akcert_e = RSA_get0_e(akcert_rsa);  // non-owning reference
  auto* akcert_n = RSA_get0_n(akcert_rsa);  // non-owning reference
  if (!akcert_e || !akcert_n) { return COUNTED_ERROR(AzureSNP_AKCertPubKey); }
  std::string ake(BN_num_bytes(akcert_e), '\0');
  ake.resize(BN_bn2bin(akcert_e, reinterpret_cast<uint8_t*>(ake.data())));
  std::string akn(BN_num_bytes(akcert_n), '\0');
  akn.resize(BN_bn2bin(akcert_n, reinterpret_cast<uint8_t*>(akn.data())));
  if (!util::ConstantTimeEquals(rte, ake) || !util::ConstantTimeEquals(rtn, akn)) {
    return COUNTED_ERROR(AzureSNP_AKCertMismatch);
  }

  LOG(DEBUG) << "AKCert verified successfully";
  return error::OK;
}

error::Error CheckRemotePCRs(context::Context* ctx, const attestation::tpm2::PCRs& local, const attestation::tpm2::PCRs& remote) {
  bool equals = 1;
  // See:
  // - https://wiki.archlinux.org/title/Trusted_Platform_Module
  // - https://uapi-group.org/specifications/specs/linux_tpm_pcr_registry/
  // Note: we verify UEFI by checking AKCert against MSFT root-of-trust, NOT by checking PCRs here.
  for (size_t i : {
      // 0,  // Core system firmware executable code (UEFI)
      // 1,  // Core system firmware data/host platform configuration (serial/model #s)
      2,  // Extended/pluggable executable code (option ROMs on pluggable hardware)
      3,  // Extended/pluggable firmware data, set during UEFI boot select
      4,  // Boot loader and additional drivers, binaries and extensions loaded by boot loader
      5,  // config to bootloaders, GPT partition table
      // 6,  // resume from S4/S5 power state events.  On Azure, this PCR contains the VM ID, a UUID, and is thus unique across all VMs.
      7,  // secure boot state, including PK/KEK/db, specific certificates used
      8,  // Kernel command line (grub and systemd-boot only)
      9,  // Kernel image, initrd, and EFI load options (kernel command line)
      // 10, // reserved for future use
      11, // hash of unified kernel image (systemd-stub)
      12, // Kernel command line, system credentials, system configuration images
      13, // System extension images for the initrd
      14, // MOK certificates and hashes
      // 15, // Userspace stuff
    }) {
    equals &= util::ConstantTimeEquals(local[i], remote[i]);
  }
  if (!equals) {
    return COUNTED_ERROR(AzureSNP_PCRMismatch);
  }
  return error::OK;
}

std::pair<std::string, error::Error> AzureRuntimeDataFromCert(X509* rsa_cert) {
  bssl::UniquePtr<EVP_PKEY> cert_pk(X509_get_pubkey(rsa_cert));
  std::string out;
  if (!cert_pk) { return std::make_pair(out, COUNTED_ERROR(AzureSNP_AKCertPubKey)); }
  auto* cert_rsa = EVP_PKEY_get0_RSA(cert_pk.get());  // non-owning reference
  if (!cert_rsa) { return std::make_pair(out, COUNTED_ERROR(AzureSNP_AKCertPubKey)); }
  auto* cert_e = RSA_get0_e(cert_rsa);  // non-owning reference
  auto* cert_n = RSA_get0_n(cert_rsa);  // non-owning reference
  if (!cert_e || !cert_n) { return std::make_pair(out, COUNTED_ERROR(AzureSNP_AKCertPubKey)); }
  std::string ake(BN_num_bytes(cert_e), '\0');
  ake.resize(BN_bn2bin(cert_e, reinterpret_cast<uint8_t*>(ake.data())));
  std::string akn(BN_num_bytes(cert_n), '\0');
  akn.resize(BN_bn2bin(cert_n, reinterpret_cast<uint8_t*>(akn.data())));
  out.append("{\"keys\":[{\"kid\":\"HCLAkPub\",\"kty\":\"RSA\",\"e\":\"");
  out.append(util::Base64Encode(ake, util::B64URL, false));
  out.append("\",\"n\":\"");
  out.append(util::Base64Encode(akn, util::B64URL, false));
  out.append("\"}]}");
  LOG(DEBUG) << "Generated Azure runtime data from X509: " << out;
  return std::make_pair(out, error::OK);
}

error::Error VerifyTPM2(context::Context* ctx, const ASNPEvidence& evidence, std::array<uint8_t, 32>* nonce, attestation::tpm2::PCRs* pcrs) {
  LOG(DEBUG) << "Verifing TPM2 quote";
  auto [sig, sigerr] = attestation::tpm2::Signature::FromString(evidence.sig());
  if (sigerr != error::OK) {
    return sigerr;
  }
  auto [msg, msgerr] = attestation::tpm2::Report::FromString(evidence.msg());
  if (msgerr != error::OK) {
    return msgerr;
  }
  if (auto err = attestation::tpm2::PCRsFromString(evidence.pcrs(), pcrs); err != error::OK) {
    return err;
  }
  auto akcert_start = reinterpret_cast<const uint8_t*>(evidence.akcert_der().data());
  bssl::UniquePtr<X509> akcert(d2i_X509(nullptr, &akcert_start, evidence.akcert_der().size()));
  if (!akcert) {
    return COUNTED_ERROR(Env_ParseEvidence);
  } else if (auto err = sig.VerifyReport(msg, akcert.get()); err != error::OK) {
    return err;
  } else if (auto err = msg.VerifyPCRs(*pcrs); err != error::OK) {
    return err;
  }
  *nonce = msg.nonce();
  return error::OK;
}

std::pair<attestation::AttestationData, error::Error> CompleteVerification(context::Context* ctx, const ASNPEvidence& evidence, const ASNPEndorsements& endorsements, util::UnixSecs now, const STACK_OF(X509)* const roots_of_trust, const attestation::tpm2::PCRs& local_pcrs) {
  attestation::AttestationData out;
  LOG(DEBUG) << "Verifing that the provided AKCert is valid and verified by the SNP report and MSFT root of trust";
  if (auto err = attestation::tpm2snp::VerifyAKCert(ctx, evidence, endorsements, now, roots_of_trust); err != error::OK) {
    return std::make_pair(out, err);
  }

  LOG(DEBUG) << "Verifing TPM2 quote";
  std::array<uint8_t, 32> nonce;
  attestation::tpm2::PCRs pcrs;
  if (auto err = VerifyTPM2(ctx, evidence, &nonce, &pcrs); err != error::OK) {
    return std::make_pair(out, err);
  }

  LOG(DEBUG) << "Verifying remote PCRs against local ones";
  if (auto err = CheckRemotePCRs(ctx, local_pcrs, pcrs); err != error::OK) {
    return std::make_pair(out, err);
  }

  LOG(DEBUG) << "Verifying that attestation data matches hash in TPM2 quote";
  if (auto ad_sha256 = hmac::Sha256(evidence.attestation_data()); !util::ConstantTimeEquals(ad_sha256, nonce)) {
    return std::make_pair(out, COUNTED_ERROR(AzureSNP_AttestationDataHashMismatch));
  }

  if (!out.ParseFromString(evidence.attestation_data())) {
    return std::make_pair(std::move(out), COUNTED_ERROR(Env_ParseEvidence));
  }
  return std::make_pair(std::move(out), error::OK);
}

}  // namespace svr2::attestation::tpm2snp
