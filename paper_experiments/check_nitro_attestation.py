import cbor2
from cryptography import x509
from cryptography.x509.verification import Store, PolicyBuilder
import datetime

from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives.asymmetric.ec import ECDSA

ATTESTATION_DOC_PATH = "nitro_attest.bin"
ROOT_CA_CERT_PATH = "nitro_attestation_pki.pem"


def decode_cose_sign1(cose_sign1):
    """
    Decode the CBOR object and map it to a COSE_Sign1 structure
    """
    cose_sign1_obj = cbor2.loads(cose_sign1)

    protected_header = cose_sign1_obj[0]
    unprotected_header = cose_sign1_obj[1]
    payload = cose_sign1_obj[2]
    signature = cose_sign1_obj[3]

    attestation_doc = cbor2.loads(payload)

    return (protected_header, unprotected_header, attestation_doc, signature)


with open(ATTESTATION_DOC_PATH, "rb") as f:
    attestation_doc_cbor = f.read()

_, _, attestation_doc, signature = decode_cose_sign1(attestation_doc_cbor)

ca_bundle = attestation_doc["cabundle"]
certificate = attestation_doc["certificate"]


def validate_certificate_chain_manual(certificate, ca_bundle, root_ca_cert_path):
    with open(root_ca_cert_path, "rb") as f:
        root_ca_cert = x509.load_pem_x509_certificate(f.read())
    certs = [x509.load_der_x509_certificate(cert) for cert in ca_bundle]  # ordered from root to leaf
    target_cert = x509.load_der_x509_certificate(certificate)

    #  verify to the root
    trusted_pub = root_ca_cert.public_key()
    for intermediate_cert in certs[1:]:
        trusted_pub.verify(intermediate_cert.signature, intermediate_cert.tbs_certificate_bytes, ECDSA(hashes.SHA384()))
        trusted_pub = intermediate_cert.public_key()

    # verify to the target
    trusted_pub.verify(target_cert.signature, target_cert.tbs_certificate_bytes, ECDSA(hashes.SHA384()))


validate_certificate_chain_manual(certificate, ca_bundle, ROOT_CA_CERT_PATH)

print("Certificate chain validated successfully!")
print()

print("Expected PCRs (from https://github.com/signalapp/libsignal/blob/main/rust/attest/src/constants.rs#L32):")
print("PCR 0: 24e56baabe26dcedc58f5753ee979e2f74750df31f43a18f0cf4e08f8ad8c0cd304142cf3441945c3568f4096cb69c66")
print("PCR 1: 52b919754e1643f4027eeee8ec39cc4a2cb931723de0c93ce5cc8d407467dc4302e86490c01c0d755acfe10dbf657546")
print("PCR 2: ec540f3f7f673ab65582d96cf26a747beffcc9392e82f48cfa4ceec47a6ad69a63f9102fc7e1fae37a83a9741814210f")
print()

# Dump the PCRs
print("PCRs from attestation document:")
print("PCR 0:", attestation_doc["pcrs"][0].hex())
print("PCR 1:", attestation_doc["pcrs"][1].hex())
print("PCR 2:", attestation_doc["pcrs"][2].hex())
